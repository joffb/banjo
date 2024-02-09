
; banjo sound driver
; Joe Kennedy 2023

.include "defines_sdas.inc"


.area _SMS_SRAM (REL,CON)

    ; console framerate 50/60hz
	music_framerate: .ds 1

	; write bytes here to queue up music or sfx
	queue_sfx: .ds 1
	queue_song: .ds 1

    fm_unit_present: .ds 1

	; state and channel data for song
	song_playing: .ds 1
    song_state: .ds _sizeof_music_state
    ; song_channels: .ds _sizeof_channel * CHANNEL_COUNT
	; song_channel_ptrs: .ds CHANNEL_COUNT * 2
	
	; state and channel data for sfx
	sfx_playing: .ds 1
	sfx_priority: .ds 1
    sfx_state: .ds _sizeof_music_state
    sfx_channel: .ds _sizeof_channel
	
    .bndry 8
    fm_drum_note_ons: .ds 1
    fm_drum_volumes: .ds 3

.area _CODE (REL,CON)

    .globl song_state, sfx_state
    .globl _song_channels, _song_channel_ptrs, sfx_channel
    .globl _song_table, _sfx_table
    .globl queue_song, song_playing, queue_sfx, sfx_playing, sfx_priority
    .globl _banjo_init, _banjo_update, _banjo_queue_song, _banjo_queue_sfx, _banjo_fm_unit_present

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

    sn_tone_lookup:
        .include "sn_fnums.inc"

    fm_tone_lookup:
        .include "fm_fnums.inc"

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
