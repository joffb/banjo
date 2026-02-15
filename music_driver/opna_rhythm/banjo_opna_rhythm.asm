
; banjo sound driver
; Joe Kennedy 2025

.include "../banjo_defines_wladx.inc"


.RAMSECTION "BANJO_OPNA_RHYTHM_RAM" APPENDTO "BANJO_RAM" FREE

	banjo_opna_rhythm_note_ons: db
    banjo_opna_rhythm_note_offs: db

.ENDS



.SECTION "BANJO_OPNA_RHYTHM" free

    .include "command_jump_table.inc"
    .include "commands.inc"
    .include "init.inc"
    .include "mute_unmute.inc"
    .include "update.inc"

.ENDS