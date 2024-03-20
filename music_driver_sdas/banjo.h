
// banjo sound driver
// Joe Kennedy 2024

#ifndef __BANJO_H
#define __BANJO_H

// instructions
#define NOTE_ON 			0x00
#define NOTE_OFF 			0x01
#define INSTRUMENT_CHANGE	0x02
#define VOLUME_CHANGE		0x03
#define FM_DRUM 			0x04
#define SN_NOISE_MODE		0x05
#define SLIDE_UP	        0x06
#define SLIDE_DOWN          0x07
#define SLIDE_PORTA		    0x08
#define SLIDE_OFF		    0x09
#define ARPEGGIO			0x0a
#define ARPEGGIO_OFF		0x0b
#define VIBRATO 			0x0c
#define VIBRATO_OFF 		0x0d
#define LEGATO_ON           0x0e
#define LEGATO_OFF          0x0f
#define GAME_GEAR_PAN       0x10
#define END_LINE            0x80

#define MODE_FM             1
#define MODE_SN             2
#define MODE_SN_FM          3
#define MODE_DUAL_SN        4
#define MODE_FM_DRUMS       5
#define MODE_SN_FM_DRUMS    7

#define CHAN_FLAG_MUTED         0x01
#define CHAN_FLAG_NOTE_ON       0x02
#define CHAN_FLAG_LEGATO        0x04
#define CHAN_FLAG_PITCH_CHANGE  0x08
#define CHAN_FLAG_VOLUME_MACRO  0x10
#define CHAN_FLAG_VIBRATO       0x20
#define CHAN_FLAG_ARPEGGIO      0x40
#define CHAN_FLAG_SLIDE         0x80

// used in song/sfx table definitions
#define SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY) { SFX_PRIORITY, SFX_BANK, &SFX_LABEL }
#define SONG_DEF(SONG_LABEL, SONG_BANK) { 0, SONG_BANK, &SONG_LABEL }

typedef struct instrument_s {

	unsigned char volume_macro_len;
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
    unsigned char subtic;
    unsigned char tic;
    unsigned char line;
    unsigned char order;
    unsigned char process_new_line;
    unsigned char noise_mode;
    unsigned char panning;
    unsigned char const channel_types[32];
} song_data_t;

typedef struct channel_s {
    unsigned char flags;
    unsigned char type;
    unsigned char subchannel;
    unsigned char port;

    unsigned int freq;
    unsigned int target_freq;

    unsigned char volume;
    unsigned char midi_note;
    unsigned char instrument_num;
            
    unsigned char slide_amount;
    unsigned char slide_type;
            
    unsigned char vibrato_current;
    unsigned char vibrato_target;
    unsigned char vibrato_counter;
    unsigned char vibrato_counter_add;

    unsigned char arpeggio_pos;
    unsigned char arpeggio;

    unsigned int order_table_ptr;
    unsigned int pattern_ptr;
    unsigned char line_wait;
            
    unsigned char volume_macro_len;
    unsigned char volume_macro_pos;
    unsigned char volume_macro_loop;
    unsigned int volume_macro_ptr;
            
    unsigned char fm_patch_shifted;
    unsigned char fm_drum_trigger;
    unsigned char fm_drum_volume_mask;
} channel_t;

typedef struct song_s {

    unsigned char priority;
    unsigned char bank;
    song_data_t * const song;
    
} song_t;

// stores the mode after it's set in banjo_init
extern unsigned char banjo_mode;

// flags whether the FM unit is installed
extern unsigned char banjo_fm_unit_present;

// flags whether we're running on a Game Gear
extern unsigned char banjo_game_gear_mode;

// flags whether we're running on Sega System E
extern unsigned char banjo_system_e;

extern song_data_t song_state;
extern channel_t song_channels[];

extern song_data_t sfx_state;
extern channel_t sfx_channel;

// sets banjo_fm_unit_present to 1 if an fm unit is installed, 0 otherwise
// sets banjo_game_gear_mode to 1 if it's detected that wer're running on a game gear in game gear mode, 0 otherwise
void banjo_check_hardware(void);

// initialise banjo for the given mode
void banjo_init(unsigned char mode);

// update song and sfx
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
void banjo_set_song_table(song_t const *song_table_ptr);
void banjo_set_sfx_table(song_t const *sfx_table_ptr);

// stop the currently playing song
void banjo_song_stop(void);

// stop the currently playing sfx
void banjo_sfx_stop(void);

// resume playback of a stopped song
void banjo_song_resume(void);

#endif