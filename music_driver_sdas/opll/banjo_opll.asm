
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_sdas.inc"

.module BANJO_OPLL

.ifeq BANJO_SYS - 1
    OPLL_REG_PORT .equ 0xf0
    OPLL_DATA_PORT .equ 0xf1
.endif
    
.ifeq BANJO_SYS - 2
    OPLL_REG_PORT .equ 0x7c
    OPLL_DATA_PORT .equ 0x7d
.endif

.area _DATA (REL,CON)

    opll_drum_note_ons: .ds 1

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif

    .globl _banjo_update_channel_opll
    .globl _banjo_update_channels_opll
    .globl _banjo_mute_all_opll
    .globl _banjo_mute_channel_opll
    .globl _banjo_init_opll
    .globl _banjo_init_sfx_channel_opll
    .globl _banjo_opll_write_delay

    ; used in opll_drum
    .globl music_update_channel_opll
    .globl opll_drum_note_ons
    .globl fm_tone_lookup

    .include "fnums_fm.inc"
    .include "command_jump_table.inc"
    .include "init.inc"
    .include "instrument_change.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "update_pitch_registers.inc"
    .include "update.inc"
    .include "volume_change.inc"

    .ifndef BANJO_MINIMAL
    .include "pitch_slide.inc"
    .endif