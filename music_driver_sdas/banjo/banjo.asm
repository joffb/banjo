
; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_sdas.inc"

.module BANJO_MAIN

.area _DATA (REL,CON)

    ; console framerate 50/60hz
	music_framerate: .ds 1

    _banjo_has_chips: .ds 1
    _banjo_system_flags: .ds 1
    _banjo_max_channels: .ds 1

	; state and channel data for song
	song_playing: .ds 1
    _song_state: .ds _sizeof_music_state

    _banjo_memory_control_value: .ds 1

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif

    .globl song_playing, _song_state, _song_channels, _song_channel_ptrs
    .globl _banjo_has_chips, _banjo_system_flags, _banjo_max_channels
    .globl _banjo_song_stop, _banjo_song_resume
    .globl _banjo_init
    .globl _banjo_play_song
    .globl _banjo_update_song
    .globl _banjo_mute_song_channel, _banjo_unmute_song_channel
    .globl bmsc_sfx_jump
    .globl _banjo_check_hardware
    .globl _banjo_set_song_loop_mode

    .ifndef BANJO_MINIMAL
    .globl _banjo_set_song_master_volume
    .globl _banjo_song_fade_out
    .globl _banjo_song_fade_in 
    .globl music_update_vibrato, music_update_arpeggio, music_update_volume_macro, music_update_ex_macro
    .endif

    .globl mpnl_command_done, mpnl_command_done_one_byte_command
    .globl mpnl_volume_change, mpnl_instrument_change
    .globl mpnl_arpeggio, mpnl_arpeggio_off
    .globl mpnl_pitch_slide_up, mpnl_pitch_slide_down, mpnl_portamento, mpnl_slide_off
    .globl mpnl_legato_on, mpnl_legato_off
    .globl mpnl_vibrato, mpnl_vibrato_off
    .globl mpnl_order_jump, mpnl_order_next
    .globl mpnl_set_speed_1, mpnl_set_speed_2
    .globl mpnl_note_delay
    .globl mpnl_skip_1_byte_command, mpnl_skip_2_byte_command, mpnl_skip_3_byte_command

    .globl music_note_on, music_note_off
    .globl music_calc_fnum
    .globl music_instrument_change
    
    .globl music_update
    .globl music_play
    .globl music_process_new_line

	.ifeq BANJO_SYS - 1
		.include "check_hardware_sms.inc"
        .include "init_sms.inc"
    .endif

	.ifeq BANJO_SYS - 2
        bch_msx_opll_magic_string: 
            .ascii /APRLOPLL/
		.include "check_hardware_msx.inc"
        .include "init_msx.inc"
	.endif


    .include "commands.inc"
    .include "init.inc"
    .include "instrument_change.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "pattern_change.inc"
    .include "play.inc"
    .include "process_new_line.inc"
    .include "stop.inc"
    .include "update_pitch_registers.inc"
    .include "update.inc"

    .ifndef BANJO_MINIMAL
    .include "arpeggio.inc"
    .include "ex_macro.inc"
    .include "master_volume.inc"
    .include "vibrato.inc"
    .include "volume_macro.inc"
    .endif