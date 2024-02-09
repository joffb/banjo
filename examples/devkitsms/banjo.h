
// banjo sound driver
// Joe Kennedy 2024

#ifndef __BANJO_H
#define __BANJO_H

#define NOTE_ON 			0x00
#define NOTE_OFF 			0x01
#define INSTRUMENT_CHANGE	0x02
#define FM_PATCH 			0x03
#define FM_DRUM 			0x04
#define SN_NOISE_MODE		0x05
#define PITCH_SLIDE_UP	    0x06
#define PITCH_SLIDE_DOWN	0x07
#define ARPEGGIO			0x08
#define PORTAMENTO		    0x09
#define LINE_WAIT			0x0a
#define END_PATTERN_LINE    0xff

#define SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY) { SFX_PRIORITY, SFX_BANK, &SFX_LABEL }
#define SONG_DEF(SONG_LABEL, SONG_BANK) { 0, SONG_BANK, &SONG_LABEL }

typedef struct instrument_s {

	unsigned char volume_macro_len;
	unsigned char volume_macro_mode;
	unsigned char volume_macro_loop;
	unsigned char const * const volume_macro_ptr;
	unsigned char fm_preset;
	unsigned char fm_patch[8];

} instrument_t;

typedef struct song_data_s {
    unsigned char magic_byte;
    unsigned char bank;
    unsigned char channel_count;
    unsigned char loop;
    unsigned char sfx_channel;
    unsigned char has_sn;
    unsigned char has_fm;
    unsigned char has_fm_drums;
    unsigned char time_base;
    unsigned char speed_1;
    unsigned char speed_2;
    unsigned char pattern_length;
    unsigned char orders_length;
    instrument_t const * const * const instrument_pointers;
    unsigned char const * const * const * order_pointers;
    unsigned char const channel_types[32];
} song_data_t;

typedef struct song_s {

    unsigned char priority;
    unsigned char bank;
    song_data_t * const song;
    
} song_t;

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

#endif