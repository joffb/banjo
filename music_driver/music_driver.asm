
; banjo sound driver
; Joe Kennedy 2023

.include "defines_wladx.inc"

.macro SFX_DEF(SFX_LABEL, SFX_PRIORITY)
	.db SFX_PRIORITY
	.db :SFX_LABEL
	.dw SFX_LABEL
.endm

.macro SONG_DEF(SONG_LABEL)
	.db 0
	.db :SONG_LABEL
	.dw SONG_LABEL
.endm

.ifdef BANJO_MODE

	; FM: 9 channels
	.if BANJO_MODE = MODE_FM

		.define CHANNEL_COUNT 9

	; SN: 4 channels
	.elif BANJO_MODE = MODE_SN

		.define CHANNEL_COUNT 4

	; Combined SN and FM: 13 channels
	.elif BANJO_MODE = MODE_SN_FM

		.define CHANNEL_COUNT 13

	; FM drums: 11 channels
	.elif BANJO_MODE = MODE_FM_DRUMS

		.define CHANNEL_COUNT 11

	; 2x SN: 8 channels
	.elif BANJO_MODE = MODE_DUAL_SN

		.define CHANNEL_COUNT 8

	; Combined SN and FM drums: 15 channels
	.elif BANJO_MODE = MODE_SN_FM_DRUMS

		.define CHANNEL_COUNT 15

	.endif

.endif

; number of channels to allocate as space for songs
.ifndef BANJO_MODE
	.define CHANNEL_COUNT 4
	.define BANJO_MODE MODE_SN
.endif

.RAMSECTION "Music Vars" slot 3

	; console framerate 50/60hz
	music_framerate: db

	; write bytes here to queue up music or sfx
	queue_sfx: db
	queue_song: db

	; used to modify the loop type
	queue_sfx_loop: db
	queue_song_loop: db

	banjo_fm_unit_present: db
	banjo_game_gear_mode: db
	banjo_system_e: db
	banjo_mode: db

	; state and channel data for song
	song_playing: db
	song_state: INSTANCEOF music_state
	song_channels: INSTANCEOF channel CHANNEL_COUNT
	song_channel_ptrs: ds CHANNEL_COUNT * 2
	
	; state and channel data for sfx
	sfx_playing: db
	sfx_priority: db
	sfx_state INSTANCEOF music_state
	sfx_channel INSTANCEOF channel

	; pointers to song and sfx tables
	song_table_ptr: dw
	sfx_table_ptr: dw

	fm_drum_note_ons: db
	fm_drum_volumes: ds 3

.ENDS

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

