
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_sdas.inc"

.module BANJO_SN

SN76489_PORT .equ 0x7f
SN76489_2_PORT .equ 0x7b
SN76489_3_PORT .equ 0x7f
SN76489_4_PORT .equ 0x7f


.globl _banjo_init_sn, _banjo_init_sn_2, _banjo_init_sn_3, _banjo_init_sn_4
.globl _banjo_init_sfx_channel_sn, _banjo_mute_all_sn
.globl _banjo_mute_channel_sn
.globl _banjo_update_channels_sn, _banjo_update_channel_sn

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif
	
	.include "fnums_sn.inc"
	.include "command_jump_table.inc"
	.include "commands.inc"
    .include "init.inc"
	.include "mute_unmute.inc"
	.include "note_on_off.inc"
	.include "pitch_slide.inc"
	.include "update.inc"
	.include "update_pitch_registers.inc"
	.include "volume_change.inc"