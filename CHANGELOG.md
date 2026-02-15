## WIP
+ Split up code so each of these is now separate with the only common dependancy being the main banjo code
    + the main part of banjo
    + SN chip code
    + OPLL chip code
    + AY chip code
    + SFX playback
    + Queue system
+ Song files themselves reference the functions they need to initialise/update/mute the song and its channels
+ Removed the C export from json2sms.py use the -sdas option and export to .asm instead (see README)
+ Added support for the AY3 sound chip
+ Added one extra macro per channel for pitch/arpeggio/duty effects
+ `banjo_init` now takes two parameters - the maximum number of channels used and the chips to use e.g.
    + `banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);`
+ `banjo_play_song` no longer takes a loop parameter, just the song pointer
+ `banjo_set_song_loop_mode` and `banjo_set_sfx_loop_mode` added to change the loop mode
+ Song channels now have to be allocated by the user rather than banjo (to reduce the number of combinations required). This can be done in C using this macro (with your global variables)
    + `BANJO_SONG_CHANNELS(CHAN_COUNT_SN + CHAN_COUNT_OPLL)`