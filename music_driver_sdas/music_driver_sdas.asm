
; banjo sound driver
; Joe Kennedy 2023

.include "defines_sdas.inc"

.area _SMS_SRAM (REL,CON)

    ; console framerate 50/60hz
	music_framerate: .ds 1

	; write bytes here to queue up music or sfx
	queue_sfx: .ds 1
	queue_song: .ds 1

	; used to modify the loop type
    queue_sfx_loop: .ds 1
    queue_song_loop: .ds 1

    _banjo_fm_unit_present: .ds 1
    _banjo_game_gear_mode: .ds 1
    _banjo_system_e: .ds 1
	_banjo_mode: .ds 1

	; state and channel data for song
	song_playing: .ds 1
    _song_state: .ds _sizeof_music_state
    _song_channels: .ds _sizeof_music_state * CHANNEL_COUNT
	song_channel_ptrs: .ds CHANNEL_COUNT * 2

	; state and channel data for sfx
	sfx_playing: .ds 1
	sfx_priority: .ds 1
    _sfx_state: .ds _sizeof_music_state
    _sfx_channel: .ds _sizeof_channel
	
	; pointers to song and sfx tables
	song_table_ptr: .ds 2
	sfx_table_ptr: .ds 2

    fm_drum_note_ons: .ds 1
    fm_drum_volumes: .ds 3

.area _CODE (REL,CON)

    .globl _song_state, _song_channels
    .globl _sfx_state, _sfx_channel
    .globl _banjo_set_song_table, _banjo_set_sfx_table
    .globl _banjo_song_stop, _banjo_sfx_stop
    .globl _banjo_init, _banjo_update, _banjo_queue_song, _banjo_queue_sfx
    .globl _banjo_check_hardware, _banjo_fm_unit_present, _banjo_mode, _banjo_game_gear_mode, _banjo_system_e
    .globl _banjo_queue_song_loop_mode, _banjo_queue_sfx_loop_mode

    .globl queue_song, song_playing
    .globl queue_sfx, sfx_playing, sfx_priority

    .include "music_init.inc"
    .include "music_play.inc"
    .include "music_stop.inc"
    .include "music_update.inc"
    .include "update_pitch_registers.inc"
    .include "instrument_change.inc"
    .include "process_channels_tic.inc"
    .include "process_new_line.inc"
    .include "note_on_off.inc"
    .include "mute_unmute.inc"
    .include "arpeggio.inc"
    .include "pitch_slide.inc"
    .include "vibrato.inc"
    .include "volume_change.inc"
    .include "volume_macro.inc"
    .include "fnums_sn.inc"
    .include "fnums_fm.inc"