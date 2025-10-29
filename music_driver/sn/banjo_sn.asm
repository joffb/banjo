
; banjo sound driver
; Joe Kennedy 2024

.include "../banjo_defines_wladx.inc"

.if BANJO_SYS == 1
	.define SN76489_PORT 0x7f
	.define SN76489_2_PORT 0x7b
	.define SN76489_3_PORT 0x7f
	.define SN76489_4_PORT 0x7b
.elif BANJO_SYS == 2
	.define SN76489_PORT 0x7f
	.define SN76489_2_PORT 0x7b
	.define SN76489_3_PORT 0x7f
	.define SN76489_4_PORT 0x7b
; ALF TEST
.elif BANJO_SYS == 9
	.define SN76489_PORT 0x7f
	.define SN76489_2_PORT 0x7e
	.define SN76489_3_PORT 0x7d
	.define SN76489_4_PORT 0x7c

	.define BANJO_ALF 1
	.define BANJO_MULTIPLE_SN 1
.endif

.SECTION "BANJO_SN" free

	.include "fnums_sn.inc"
	.include "command_jump_table.inc"
	.include "commands.inc"    
    .include "init.inc"
	.include "mute_unmute.inc"
	.include "note_on_off.inc"
	.include "update.inc"
	.include "update_pitch_registers.inc"
	.include "volume_change.inc"

.ENDS