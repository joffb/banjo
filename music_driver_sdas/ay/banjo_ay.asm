
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_sdas.inc"

.module BANJO_AY

AY_REG_WRITE .equ 0xa0
AY_DATA_WRITE .equ 0xa1
AY_DATA_READ .equ 0xa2

.area _CODE (REL,CON)

	.globl _banjo_init_ay, _banjo_init_dual_ay
	.globl _banjo_init_sfx_channel_ay, _banjo_mute_all_ay
	.globl _banjo_mute_channel_ay
	.globl _banjo_update_channels_ay, _banjo_update_channel_ay

	.include "commands.inc"
    .include "commands_envelopes.inc"
	.include "command_jump_table.inc"
    .include "fnums_ay.inc"
    .include "init.inc"
	.include "mute_unmute.inc"
	.include "note_on_off.inc"
	.include "pitch_slide.inc"
	.include "update.inc"
	.include "update_pitch_registers.inc"
	.include "volume_change.inc"