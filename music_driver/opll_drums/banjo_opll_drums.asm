
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.RAMSECTION "BANJO_OPLL_DRUMS_RAM" APPENDTO "BANJO_RAM" FREE

    opll_drum_fixed_pitch_mode: db

    opll_drum_pitch_register: db
	opll_drum_trigger: db
	opll_drum_volume_mask: db

	opll_drum_volumes: ds 3

.ENDS

.SECTION "BANJO_OPLL_DRUMS" free

    .include "commands.inc"
    .include "command_jump_table.inc"
    .include "drum_tables.inc"
    .include "init.inc"
    .include "mute_unmute.inc"
    .include "note_on_off.inc"
    .include "update.inc"
    .include "volume_change.inc"

.ENDS