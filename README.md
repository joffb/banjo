# Banjo sound driver for Sega Master System

### Features:
+ Furnace tracker file conversion
+ WLA-DX and SDCC/SDAS support
+ SN76489 PSG, YM2413 FM, and dual SN76489 support (Sega System E)
+ Song playback
+ Sound Effect playback
    + only 1 SFX can be played back at a time
    + only 1 channel can be included in the SFX
    + SFX can be given a priority, so one SFX can override another if it's more important, or is ignored if it's not
+ Simple system for playing back songs and SFX, so you only need to call `banjo_queue_song(song_number);` or do `ld a, song_number / ld (queue_song), a` and the next call to `banjo_update` will handle it.
+ Supports Song/SFX sizes up to 16kb (the song needs to be entirely in one bank)

### Furnace features and effects currently supported:
**NB:** *Compatibility Flags > Pitch/Playback > Pitch Linearity* should be set to "None" in Furnace
+ Volume macros (only "sequence" mode)
+ YM2413 custom patches and custom drum pitches
+ Effects:
    + Arpeggios (00)
    + Pitch slides up and down (01, 02)
    + Portamento (03)
    + Vibrato (04)
    + SN76489 noise mode (20)
    + Game Gear stereo panning (80)

### furnace2json
This is a script to turn a furnace .fur file into a json representation
`python furnace2json.py input.fur output.json`

### json2sms
This is a script to turn the json output of furnace2json into a .c or .asm file

For songs, you use it like:

```python json2sms.py song_name input.json output.asm```

```python json2sms.py song_name input.json output.c```

"song_name" will then be used as the name of the variable in .c or label for the song in .asm

For sfx you use it like:

```python json2sms.py sfx 2 sfx_name input.json output.asm```

```python json2sms.py sfx 2 sfx_name input.json output.c```

"sfx" as the first parameter indicates that it should run in sfx mode

SFX only run on one channel, specified by the second parameter. So here "2" is the channel number which the sfx is in (zero indexed, so the first channel in the .fur is 0). Other channels will not be included in the export!

To make channel numbers line up properly with Song playback, your SFX .fur should also use the same chip and channel configuration as the Song .fur - i.e. if the Song uses both SN and FM, the SFX should also use both SN and FM so the channel numbers are the same in both cases.

### Libraries

For SDCC/SDAS, the .rel libraries are located in music_driver_sdas/lib

The major difference between the library files is that they reserve different amounts of space in RAM for song channels, to accommodate songs for different types of hardware.
You should choose the library which provides enough space to handle all of the channels in your songs, for all supported configurations.

| music_driver_ | FM  | SN  | SN_FM | DUAL_SN | FM_DRUMS | SN_FM_DRUMS | 
|---------------|-----|-----|-------|---------|----------|-------------|
| Largest song type supported | YM2413 | SN76489 | YM2413 + SN76489 | Dual SN76489 | YM2413 in percussion mode | SN76489 + YM2413 in percussion mode |
| Channel count | 9   | 4   | 13    | 8       | 11       | 15          |
| RAM used (bytes)  | 288 | 128 | 416   | 256     | 352      | 480         |

As an example, music_driver_fm.rel has 9 channels so it has enough space to handle playback of FM, SN and Dual SN songs.
If you used this to play back a song with FM percussion, it expects 11 channels worth of RAM (2 more than the alloted 9) so updates to the last two channels could spill over into RAM used for your software and cause havoc, so try to avoid doing that!

## Using the sound driver

### Hardware detection

If you want to support multiple hardware configurations, you can use `banjo_check_hardware();` or `call banjo_check_hardware` to determine whether an FM Unit is present, and whether your software is running on a Game Gear in Game Gear mode. This call will set the `banjo_fm_unit_present` and `banjo_game_gear_mode` global variables.

### Initialisation

After possibly using the outcome of hardware detection to decide which type of songs you'll be playing, you can then use `banjo_init(MODE);` or `ld a, MODE / call banjo_init` to intialise the sound driver. This initialises variables which are used by the driver, sets up pointers to channel data, and will switch the FM and SN sound generators on or off using the Audio Control Port (0xf2) depending on the mode. It also stores the mode in the `banjo_mode` global variable.

| Mode define | Value |
|-------------|-------|
| MODE_FM  | 1 |
| MODE_SN  | 2 |
| MODE_SN_FM | 3
| MODE_DUAL_SN | 4 |
| MODE_FM_DRUMS | 5 |
| MODE_SN_FM_DRUMS |7 |

The numbers are organised so the bottom two bits can be isolated and sent to the Audio Control port (where bit 0 enables FM and bit 1 enables SN).

### Song/sfx Tables

The sound driver uses tables of pointers to Song/SFX data.
e.g. in C:
```c
    // References to Songs in other .c files which will be matched up at link time
    extern song_data_t const title_screen_song;
    extern song_data_t const town_song;

    // Song table
    // SONG_DEF(SONG_LABEL, SONG_BANK)
    song_t const song_table[] = {
        SONG_DEF(title_screen_song, 2),
        SONG_DEF(town_song, 2),
    };
```

The locations of these tables should be set with the `banjo_set_song_table(song_table_ptr);` and `banjo_set_sfx_table(sfx_table_ptr);` functions. Depending on what `banjo_check_hardware`found you can decide which table to load (e.g. if no FM unit is present, use the SN song and sfx tables instead of the FM ones).

To queue up a song or sfx, you use `banjo_queue_song(song);` or `banjo_queue_song(sfx);`. The values passed to these should be the index of the song in the table. Using the above table in the C code as an example, `banjo_queue_song(0);` would queue up the title_screen_song, `banjo_queue_song(1);` the town_song.


If your software supports multiple hardware configurations, it would be best to have a Song and SFX table per hardware configuration.

The advantage of having separate tables for different hardware is that you could do something like this:

```c
    // do we have an fm unit? use the _fm table
    if (banjo_fm_unit_present)
    {
        banjo_set_song_table(song_table_fm);
    }
    // otherwise use the _sn table
    else
    {
        banjo_set_song_table(song_table_sn);
    }

    // queue title screen song
    banjo_queue_song(0);
```

If for example both tables are set up so that song 0 is the title screen song (arranged specifically for each hardware configuration) you only need to set the tables once - all subsequent `banjo_queue_` calls can be exactly the same.

The `banjo_queue_` functions don't immediately start playback of the new SFX or Song - it happens in `banjo_update()`:

### banjo_update()

`banjo_update();` (or `call banjo_update`) should be run at every vblank. It's better to run it *after* any code which updates the VDP, to give that code as much time in vblank as possible.

It's in this function that the queues are checked and playback of a new Song or SFX is started. For SFX, there is an added check of the SFX priority - SFX with lower priority won't start playing if an SFX with higher priority is already playing. If a Song or SFX isn't queued, it updates the ones which are currently playing.

`banjo_update` will also change the bank for slot 2 (0x8000 - 0xbfff) so if you're relying on something being in bank 2 you'll have to change the bank afterwards.

### Song data
The converted Song/SFX data is similar to the tracker data. Each Channel has an Order array which has an array of pointers to Patterns in the order they're to be played back.

A Pattern for a Channel is an array of bytes which map to commands. The end of a line is signified by END_PATTERN_LINE (0xff) or LINE_WAIT (0xa). END_PATTERN_LINE says the current tracker Line is done so we can stop processing this channel's commands until the next Line is reached. LINE_WAIT stops processing the channel's commands and waits for a variable number of Lines before processing begins again. A list of commands in the pattern might look like:

`INSTRUMENT_CHANGE, 3, ARPEGGIO, 5, 12, NOTE_ON, 15, LINE_END + 15 `

Which means change the channel's instrument to 3, start an arpeggio that goes [0, 5, 12], start playing note 15 at full volume (the value 176 has all the bits set so it can be sent straight to the chip to update the volume) and then wait for 15 lines.

## Functions

```c
// initialise banjo for the given mode
void banjo_init(unsigned char mode);

// update song and sfx
// this will change the bank for slot 2 (i.e. 0x8000 to 0xbfff (writes to mapper at 0xffff))
void banjo_update();

// queue song/sfx to be played back starting on next banjo_update
void banjo_queue_song(unsigned char song);
void banjo_queue_sfx(unsigned char sfx);

// set the loop mode for the song/sfx
// by default songs loop, but sfx don't
void banjo_queue_song_loop_mode(unsigned char loop);
void banjo_queue_sfx_loop_mode(unsigned char loop);

// set up pointers to the song and sfx tables
void banjo_set_song_table(song_t const *song_table_ptr);
void banjo_set_sfx_table(song_t const *sfx_table_ptr);

// stop the currently playing sfx
void banjo_sfx_stop();

// sets banjo_fm_unit_present to 1 if an fm unit is installed, 0 otherwise
// sets banjo_game_gear_mode to 1 if it's detected that wer're running on a game gear in game gear mode, 0 otherwise
void banjo_check_hardware();

// stores the mode after it's set in banjo_init
extern unsigned char banjo_mode;

// flags whether the fm unit is installed
extern unsigned char banjo_fm_unit_present;

// flags whether we're running on a game gear
extern unsigned char banjo_game_gear_mode;
```