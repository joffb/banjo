
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.define AY_REG_WRITE 0xa0
.define AY_DATA_WRITE 0xa1
.define AY_DATA_READ 0xa2

.SECTION "BANJO_AY" free

	.include "fnums_ay.inc"
	.include "command_jump_table.inc"
	.include "commands.inc"
	.include "commands_envelopes.inc"
	.include "init.inc"
	.include "mute_unmute.inc"
	.include "note_on_off.inc"
	.include "update.inc"
	.include "update_pitch_registers.inc"
	.include "volume_change.inc"

.ENDS