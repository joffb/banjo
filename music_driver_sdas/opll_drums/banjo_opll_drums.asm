
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_sdas.inc"

.module BANJO_OPLL_DRUMS

.ifeq BANJO_SYS - 1
    OPLL_REG_PORT .equ 0xf0
    OPLL_DATA_PORT .equ 0xf1
.endif
    
.ifeq BANJO_SYS - 2
    OPLL_REG_PORT .equ 0x7c
    OPLL_DATA_PORT .equ 0x7d
.endif

.area _DATA (REL,CON)

    opll_drum_fixed_pitch_mode: .ds 1

    opll_drum_pitch_register: .ds 1
	opll_drum_trigger: .ds 1
	opll_drum_volume_mask: .ds 1
    
	opll_drum_volumes: .ds 3

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif

    .globl _banjo_update_channel_opll_drums
    .globl _banjo_update_channels_opll_drums
    .globl _banjo_mute_all_opll_drums
    .globl _banjo_mute_channel_opll_drums
    .globl _banjo_init_opll_drums

    .include "commands.inc"
    .include "drum_tables.inc"
    .include "init.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "pitch_slide.inc"
    .include "update.inc"
    .include "volume_change.inc"