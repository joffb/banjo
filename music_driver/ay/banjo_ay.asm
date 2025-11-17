
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
	.define AY_REG_WRITE 0xa0
	.define AY_DATA_WRITE 0xa1
	.define AY_DATA_READ 0xa2
.elif BANJO_SYS == 2
	.define AY_REG_WRITE 0xa0
	.define AY_DATA_WRITE 0xa1
	.define AY_DATA_READ 0xa2
.elif BANJO_SYS == 3
	.define AY_REG_WRITE 0x44
	.define AY_DATA_WRITE 0x45
	.define AY_DATA_READ 0x45
.endif

.SECTION "BANJO_AY" free

	.ifdef BANJO_3_57MHZ
	.include "fnums_3p57mhz.inc"
	.endif

	.ifdef BANJO_4MHZ
	.include "fnums_4mhz.inc"
	.endif
	
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