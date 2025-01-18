
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
    .define OPLL_REG_PORT 0xf0
    .define OPLL_DATA_PORT 0xf1
.elif BANJO_SYS == 2
    .define OPLL_REG_PORT 0x7c
    .define OPLL_DATA_PORT 0x7d
.endif

.RAMSECTION "BANJO_OPLL_RAM" APPENDTO "BANJO_RAM" FREE

	opll_drum_note_ons: db

.ENDS


.SECTION "BANJO_OPLL" free

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

.ENDS