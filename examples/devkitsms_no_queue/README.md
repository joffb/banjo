# DevkitSMS example not using the queue system
This will play a song in the background (different songs depending on whether an FM chip is detected)
You can press button 2 to play a sound effect

This doesn't use the built in queues system, instead it manually sets the bank and uses `banjo_play_song/sfx` and `banjo_update_song/sfx`.

To build this you'll need to copy crt0_sms.rel, SMSlib.h and SMSlib.lib from DevkitSMS into this folder (or add the folders containing them to the include path and library include paths in the build scripts)