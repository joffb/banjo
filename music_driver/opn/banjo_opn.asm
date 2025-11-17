
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
    .define OPN_REG_PORT 0x14
    .define OPN_DATA_PORT 0x15
    .define OPNA_REG_PORT 0x16
    .define OPNA_DATA_PORT 0x17
.elif BANJO_SYS == 2
    .define OPN_REG_PORT 0x14
    .define OPN_DATA_PORT 0x15
    .define OPNA_REG_PORT 0x16
    .define OPNA_DATA_PORT 0x17
.elif BANJO_SYS == 3
    .define OPN_REG_PORT 0xa8
    .define OPN_DATA_PORT 0xa9
    .define OPNA_REG_PORT 0xac
    .define OPNA_DATA_PORT 0xad
.endif

.RAMSECTION "BANJO_OPN_RAM" APPENDTO "BANJO_RAM" FREE

	banjo_opn_port: db
    banjo_opna_port: db

.ENDS


.SECTION "BANJO_OPN" free

    .ifdef BANJO_3_57MHZ
        .include "fnums_3p57mhz.inc"
    .endif

    .ifdef BANJO_4MHZ
        .include "fnums_4mhz.inc"
    .endif

    .include "command_jump_table.inc"
    .include "commands.inc"
    .include "init.inc"
    .include "instrument_change.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "update_pitch_registers.inc"
    .include "update.inc"
    .include "volume_change.inc"
    .include "write_calls.inc"

.ENDS