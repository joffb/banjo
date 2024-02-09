# Banjo sound driver for Sega Master System

### Features:
+ Furnace tracker file conversion
+ WLA-DX and SDCC/SDAS support
+ SN76489 PSG, YM2413 FM, and dual SN76489 support (Sega System E)
+ Song playback
+ Sound Effect playback
    + currently only 1 SFX can be played back at a time
    + currently only 1 channel can be included in the SFX
    + SFX can be given a priority, so one SFX can override another if it's more important, or is ignored if it's not
+ Simple system for playing back songs and SFX, so you only need to call `banjo_queue_song(song_number);` or do `ld a, song_number / ld (queue_song), a` and the next call to `banjo_update` will handle it.

### Furnace effects currently supported:
**NB:** *Compatibility Flags > Pitch/Playback > Pitch Linearity* should be set to "None" in Furnace
+ Volume macros (only "sequence" mode)
+ YM2413 custom patches and custom drum pitches
+ Effects:
    + Arpeggios (00)
    + Pitch slides up and down (01, 02)
    + Portamento (03)
    + SN76489 noise mode (20)

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
"2" is the channel number which the sfx is in (zero indexed, so the first channel in the .fur is 0). The other channels will not be included!

### Functions

```
// initialise banjo
void banjo_init(unsigned char channel_count);

// update song and sfx
// this will change the bank for slot 2 (i.e. 0x8000 to 0xbfff (writes to mapper at 0xffff))
void banjo_update();

// queue song to be played back starting on next banjo_update
void banjo_queue_song(unsigned char song);

// queue sfx to be played back starting on next banjo_update
void banjo_queue_sfx(unsigned char sfx);

// returns 1 if an fm unit is present, 0 otherwise
unsigned char banjo_fm_unit_present();
```