# WLA_DX example  not using the queue system
This will play a song in the background (different songs depending on whether an FM chip is detected)
You can press button 2 to play a sound effect

This doesn't use the built in queues system, instead it manually sets the bank and uses `banjo_play_song/sfx` and `banjo_update_song/sfx`.

`banjo_play_song/sfx` have the 1 byte loop variable on the stack, so as in the example you'll need to push it then `inc sp` so only a byte's worth of data is effectively pushed.