
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
    .define OPL_REG_PORT 0xc0
    .define OPL_DATA_PORT 0xc1
.elif BANJO_SYS == 2
    .define OPL_REG_PORT 0xc0
    .define OPL_DATA_PORT 0xc1
.endif

.RAMSECTION "BANJO_OPL_2OP_RAM" APPENDTO "BANJO_RAM" FREE

	opll_drum_note_ons: db

.ENDS


.SECTION "BANJO_OPL_2OP" free

    .include "fnums_fm.inc"
    .include "command_jump_table.inc"
    .include "commands.inc"
    .include "init.inc"
    .include "instrument_change.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "update_pitch_registers.inc"
    .include "update.inc"
    .include "volume_change.inc"

.ENDS