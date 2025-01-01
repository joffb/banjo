
// banjo sound driver
// Joe Kennedy 2024

#ifndef __BANJO_QUEUE_H
#define __BANJO_QUEUE_H

// used in song/sfx table definitions
#define SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY) { SFX_PRIORITY, SFX_BANK, &SFX_LABEL }
#define SONG_DEF(SONG_LABEL, SONG_BANK) { 0, SONG_BANK, &SONG_LABEL }

typedef struct song_s {

    unsigned char priority;
    unsigned char bank;
    const song_data_t * song;
    
} song_t;

// Queues ON:

// intialise queue variables
void banjo_queue_init(void);

// handle song/sfx queues and update playing song/sfx
// this will change the bank for slot 2 (i.e. 0x8000 to 0xbfff (writes to mapper at 0xffff))
void banjo_update(void);

// queue song/sfx to be played back starting on next banjo_update
void banjo_queue_song(unsigned char song);
void banjo_queue_sfx(unsigned char sfx);

// set the loop mode for the song/sfx
// by default songs loop, but sfx don't
void banjo_queue_song_loop_mode(unsigned char loop);
void banjo_queue_sfx_loop_mode(unsigned char loop);

// set up pointers to the song and sfx tables
void banjo_set_song_table(const song_t *song_table_ptr);
void banjo_set_sfx_table(const song_t *sfx_table_ptr);

#endif