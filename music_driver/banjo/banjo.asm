
; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_wladx.inc"

.RAMSECTION "BANJO_RAM" free

	; console framerate 50/60hz
	music_framerate: db

	banjo_has_chips: db
	banjo_system_flags: db
	banjo_max_channels: db

	banjo_current_channel: dw

	; state and channel data for song
	song_playing: db
	song_state: INSTANCEOF music_state

	banjo_memory_control_value: db

.ENDS

.SECTION "BANJO" free
	
	.if BANJO_SYS == 1
		.define BANJO_SMS 1
		.include "check_hardware_sms.inc"
		.include "init_sms.inc"
	.elif BANJO_SYS == 2
		.define BANJO_MSX 1
		bch_msx_opll_magic_string: 
			.db "APRLOPLL"
		.include "check_hardware_msx.inc"
		.include "init_msx.inc"
		
	; ALF TEST
	.elif BANJO_SYS == 9
		.include "check_hardware_sms.inc"
		.include "init_sms.inc"		
	.endif

	.include "init.inc"
	.include "play.inc"
	.include "stop.inc"
	.include "update.inc"
	.include "instrument_change.inc"
	.include "note_on_off.inc"
	.include "pattern_change.inc"
	.include "process_new_line.inc"
	.include "commands.inc"
	.include "mute_unmute.inc"
	.include "update_pitch_registers.inc"

	.ifndef BANJO_MINIMAL
	.include "arpeggio.inc"
	.include "master_volume.inc"
	.include "vibrato.inc"
	.include "volume_calculate.inc"
	.include "volume_macro.inc"
	.include "arp_macro.inc"
	.include "ex_macro.inc"
	.include "slides.inc"
	.include "divmod12.inc"
	.include "fnum_lookup.inc"
	.endif

.ENDS