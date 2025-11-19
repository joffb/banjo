
; banjo sound driver
; Joe Kennedy 2025

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
    .define OPM_REG_PORT 0xc0
    .define OPM_DATA_PORT 0xc1
.elif BANJO_SYS == 2
    .define OPM_REG_PORT 0x3ff0
    .define OPM_DATA_PORT 0x3ff1
.endif


.SECTION "BANJO_OPM" free

    .include "command_jump_table.inc"
    .include "commands.inc"
    .include "divmod_12_opm.inc"
    .include "init.inc"
    .include "instrument_change.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "update_pitch_registers.inc"
    .include "update.inc"
    .include "volume_change.inc"
    .include "write_calls.inc"

.ENDS