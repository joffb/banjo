
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

; number of channels to allocate as space for songs
.ifndef CHANNEL_COUNT
.define CHANNEL_COUNT 4
.endif

.RAMSECTION "Music Vars" slot 3

	; console framerate 50/60hz
	music_framerate: db

	; write bytes here to queue up music or sfx
	queue_sfx: db
	queue_song: db

	fm_unit_present: db

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
		
.ENDS

.RAMSECTION "Music FM Drum Vars" slot 3 align 8
	fm_drum_note_ons: db
	fm_drum_volumes: ds 3
.ENDS

.include "music_init.inc"
.include "music_play.inc"
.include "music_update.inc"
.include "update_pitch_registers.inc"
.include "instrument_change.inc"
.include "process_channels_tic.inc"
.include "process_new_line.inc"
.include "pitch_slide.inc"
.include "note_on_off.inc"
.include "mute_unmute.inc"

music_process_jump_table:
	.dw mpnl_note_on
	.dw mpnl_note_off
	.dw mpnl_instrument_change
	.dw mpnl_instruction_check_done
	.dw mpnl_fm_drum
	.dw mpnl_noise_mode
	.dw mpnl_pitch_slide_up
	.dw mpnl_pitch_slide_down
	.dw mpnl_arpeggio
	.dw mpnl_portamento
	.dw mpnl_line_wait

sn_tone_lookup:
	.include "sn_fnums.inc"

fm_tone_lookup:
	.include "fm_fnums.inc"

fm_drum_triggers:
	; kick
	.db 0x10
	; snare
	.db 0x08
	; tom
	.db 0x04
	; cymbal
	.db 0x02
	; hi-hat
	.db 0x01

fm_drum_pitch_registers:
	; kick
	.db 0x16
	; snare
	.db 0x17
	; tom
	.db 0x18
	; cymbal
	.db 0x18
	; hi-hat
	.db 0x17

; masks used to preserve volumes for other drums which share the byte
fm_drum_volume_masks:
	; kick
	.db 0xf0
	; snare
	.db 0xf0
	; tom
	.db 0x0f
	; cymbal
	.db 0xf0
	; hi-hat
	.db 0x0f
