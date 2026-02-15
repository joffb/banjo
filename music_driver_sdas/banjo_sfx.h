
// banjo sound driver
// Joe Kennedy 2024

#ifndef __BANJO_SFX_H
#define __BANJO_SFX_H

extern unsigned char sfx_playing;
extern song_data_t sfx_state;
extern channel_t sfx_channel;

// initialises sfx variables
void banjo_sfx_init(void);

// start playing sfx
// you should change to the sfx's bank before calling this
void banjo_play_sfx(const song_data_t *song_ptr);

// update sfx if one is playing
// change to the sfx's bank before calling this
void banjo_update_sfx(void);

// stop the currently playing sfx
void banjo_sfx_stop(void);

// set master volume for sfx playback
// values >= 0x80 are full volume
// lower values attenuate the volume
void banjo_set_sfx_master_volume(unsigned char volume);

// set sfx loop mode
// 	   0 for looping off
// 	>= 1 for looping on
void banjo_set_sfx_loop_mode(unsigned char mode);

#endif