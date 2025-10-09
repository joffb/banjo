# Banjo Sound Driver for Z80 Systems

### Features:
+ Furnace tracker file conversion
+ WLA-DX and SDCC/SDAS support
+ Chips supported:
    + SN76489 PSG
    + YM2413 OPLL FM
    + AY-3-8910 PSG
+ Intended for use in homebrew games
    + Playback takes more CPU time than VGM but song data uses a lot less ROM space
    + A limited number of effects are supported to keep code size and RAM usage low

### Furnace features and effects currently supported:
**NB:** *Compatibility Flags > Pitch/Playback > Pitch Linearity* should be set to "None" in Furnace
+ The "Speeds" tempo mode (with separate Speed 1 and Speed 2 parameters) is supported
+ Volume macro per channel (only "sequence" mode)
+ One extra macro per channel (only "sequence" mode) supporting:
    + Pitch
    + Arpeggio
    + FM Patch (OPLL)
    + Noise Freq (AY3)
    + Duty (SN)
+ YM2413 custom patches and custom drum pitches
+ Effects (multiple effects per channel are supported):
    + Arpeggios (00)
    + Pitch slides up and down (01, 02)
    + Portamento (03)
    + Vibrato (04)
    + Set speed 1 (09)
    + Set speed 2 (0F)
    + Jump to pattern (0B)
    + Jump to next pattern (0D)
        + NB: ignores the parameter and goes to the start of the next pattern
    + Note delay (ED)
        + NB: delays longer than the current line's speed will not work properly
    + SN76489
        + Noise mode (20)
        + Game Gear stereo panning (08, 80)
    + AY-3-8910
        + Set channel mode (20)
        + Set noise frequency (21)
        + Set envelope mode (22)
        + Set envelope low byte (23)
        + Set envelope high byte (24)
        + Set auto envelope (29)

## Notes for working with chips
### AY3
The master volume control will not affect channels which use envelopes - this is a hardware limitation where either the channel volume or the envelope volume is active.
### OPLL Drums
Currently only the Pattern Jump, Note Delay and Set Speed effects are supported. Macros are not supported.

### furnace2json
This is a script to turn a furnace .fur file into a json representation

`python furnace2json.py -o output.json input.fur`

### json2sms

This is a script to turn the json output of furnace2json into an .asm file.
The -i parameter can be used to determine the name of the song's variable in C or its label in Asm.

For song in a WLA-DX project, you could use it like:

```python json2sms.py -i song_name -o output.asm input.json ```

For a banked SDCC/SDAS output you could use it like:

```python json2sms.py -i song_name --sdas -a LIT_ -b 3 -o output.asm input.json ```

You can use the --sdas option to enable SDCC/SDAS compatible output for use in C projects (e.g. with devkitSMS or GBDK) and will prefix `song_name` with an underscore like `_song_name` so that it can be properly referenced from C.

The -b option can be used to set the bank number, and -a can be used to set the area name. When both an area name and bank number are provided as in the example, it will concatenate the two and output an AREA assembler directive like:

```.AREA LIT_3```

For sfx you use it with "-s" like:

```python json2sms.py -i sfx_name -s 2 -o output.asm input.json ```

SFX only play back on one channel, specified by the -s parameter. So here "-s 2" the channel number which the sfx is in (zero indexed, so the first channel in the .fur is 0). Other channels will not be included in the export!

To make channel numbers line up properly with Song playback, your SFX .fur should also use the same chip and channel configuration as the Song .fur - i.e. if the Song uses both SN and FM, the SFX should also use both SN and FM so the channel numbers are the same in both cases.

### Libraries

Banjo has its features broken up into multiple libraries:

| Library | Info |
|---------|------|
| banjo | mandatory library - has the core functionality |
| banjo_sn | support for the SN chip |
| banjo_opll | support for the OPLL chip |
| banjo_opll_drums | support for the OPLL chip in drums mode (requires banjo_opll) |
| banjo_ay | support for the AY3 chip |
| banjo_sfx | library for playing back a one channel sfx (e.g. a jump sound or a laser shot) |
| banjo_queue | library for queueing music and sound effects (requires banjo_sfx) |

### devkitSMS
lib/devkitsms has prebuilt libraries for use with devkitSMS.
Banjo's code will be in the _CODE area.
Banjo's variables will be in the _DATA area.

## GBDK
lib/gbdk_sms has prebuilt libraries for use with GBDK.
Banjo's code will be in the _CODE_1 area.
Banjo's variables will be in the _DATA area.

## Using the sound driver

### Channel definitions

As songs with different chip combinations could use a variety of channel counts, you will have to define variables yourself for the maximum number of channels you will use. The CHAN_COUNT_SN, CHAN_COUNT_AY, CHAN_COUNT_OPLL and CHAN_COUNT_OPLL_DRUMS constants can be used.

The example below uses a C macro to declare the channel variables. There are enough channels allocated to play back songs using only the SN, using only the OPLL and using a combination of SN and OPLL. However, there are not enough channels allocated to use a combination of SN and OPLL with drums.

```c
BANJO_SONG_CHANNELS(CHAN_COUNT_SN + CHAN_COUNT_OPLL)

// equivalent to:
channel_t song_channels[CHAN_COUNT_SN + CHAN_COUNT_OPLL];
channel_t *song_channel_ptrs[CHAN_COUNT_SN + CHAN_COUNT_OPLL];
```

### Initialisation

Use `banjo_check_hardware();` or `call banjo_check_hardware` to determine which hardware your system has. This will set the `banjo_has_chips` variable with different bits set to 1 if the chip is available.

The `banjo_has_chips` variable along with the BANJO_HAS_X constants can be used to intialise the sound driver using the `banjo_init(channel_count, channel_flags)` function. This initialises variables which are used by the driver and will enable the correct chips depending on the BANJO_HAS_X flags passed in.

```c
banjo_check_hardware();

if (banjo_has_chips & BANJO_HAS_OPLL)
{
    banjo_init(CHAN_COUNT_OPLL, BANJO_HAS_OPLL);
    banjo_play_song(opll_tune);
}
else if (banjo_has_chips & BANJO_HAS_AY)
{
    banjo_init(CHAN_COUNT_AY, BANJO_HAS_AY);
    banjo_play_song(ay_tune);
}
else if (banjo_has_chips & BANJO_HAS_SN)
{
    banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);
    banjo_play_song(sn_tune);
}
```

You may also want to use the `banjo_system_flags` variable along with the SYS_FLAG_X constants to play different songs e.g.

```c
// Play a different song in Game Gear mode
if (banjo_system_flags & SYS_FLAG_GG)
{
    banjo_play_song(game_gear_tune);
}
```

## Master Volume
Master volume can be set for Song and SFX playback using the `banjo_set_song_master_volume` and `banjo_set_sfx_master_volume` functions. Values >= 0x80 will be full volume, anything smaller will attenuate it.

### Looping
By default Songs will loop and SFX will not. The loop mode can be changed with the `banjo_set_song_loop_mode` and `banjo_set_sfx_loop_mode` functions. Passing 0 will switch looping off and anything >=1 will switch looping on.

### Song data
The converted Song/SFX data is similar to the tracker data. Each Channel has an Order array which has an array of pointers to Patterns in the order they're to be played back.

A Pattern for a Channel is an array of bytes which map to commands. The end of a line is signified by a byte with bit 7 set, so anything >= 0x80 (LINE_WAIT). When this is reached it stops processing the channel's commands and waits for a variable number of Lines before processing begins again, with the number of lines to wait defined by the lower 7 bits. A list of commands in the pattern might look like:

`INSTRUMENT_CHANGE, 3, ARPEGGIO, 87, NOTE_ON, 15, (LINE_END | 15), NOTE_OFF`

Which would mean change the channel's instrument to 3, start an arpeggio that goes [0, 5, 7] (87 == 0x57), start playing note number 15, stop processing this line and wait for 15 lines, then stop playing the note.