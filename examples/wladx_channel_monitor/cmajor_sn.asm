cmajor_sn:
cmajor_sn_magic_byte:
.db 186
cmajor_sn_bank:
.db 0
cmajor_sn_channel_count:
.db 4
cmajor_sn_loop:
.db 1
cmajor_sn_sfx_channel:
.db 255
cmajor_sn_has_sn:
.db 1
cmajor_sn_has_fm:
.db 0
cmajor_sn_has_fm_drums:
.db 0
cmajor_sn_time_base:
.db 2
cmajor_sn_speed_1:
.db 6
cmajor_sn_speed_2:
.db 6
cmajor_sn_pattern_length:
.db 16
cmajor_sn_orders_length:
.db 4
cmajor_sn_instrument_ptrs:
.dw cmajor_sn_instrument_pointers
cmajor_sn_order_ptrs:
.dw cmajor_sn_order_pointers
cmajor_sn_channel_types:
.db 0, 0, 0, 1, 0, 2, 0, 3, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255


cmajor_sn_volume_macros:
cmajor_sn_volume_macro_0:
.db 0, 0, 0, 1, 5, 8, 13, 15, 15
cmajor_sn_volume_macro_1:
.db 0, 0, 0, 2, 6, 8, 10, 11, 13, 13, 14, 15, 15, 15, 15
cmajor_sn_volume_macro_2:
.db 2, 7, 13, 15
cmajor_sn_volume_macro_3:
.db 10, 9, 8, 7, 6, 5, 4, 5, 6, 7, 8, 9
cmajor_sn_volume_macro_4:
.db 3, 5, 6, 7, 8, 11, 15
cmajor_sn_volume_macro_5:
.db 2, 5, 3, 8, 5, 11, 7, 13, 10, 14, 15
cmajor_sn_volume_macro_6:
.db 1, 5, 9, 10, 12, 13, 14, 14
cmajor_sn_instrument_pointers:
.dw cmajor_sn_instrument_0
.dw cmajor_sn_instrument_1
.dw cmajor_sn_instrument_2
.dw cmajor_sn_instrument_3
.dw cmajor_sn_instrument_4
.dw cmajor_sn_instrument_5
.dw cmajor_sn_instrument_6
.dw cmajor_sn_instrument_7
.dw cmajor_sn_instrument_8
.dw cmajor_sn_instrument_9
.dw cmajor_sn_instrument_10
.dw cmajor_sn_instrument_11
.dw cmajor_sn_instrument_12


cmajor_sn_instrument_data:
cmajor_sn_instrument_0:
	.db 9, 0, 4
	.dw cmajor_sn_volume_macro_0
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_1:
	.db 15, 0, 255
	.dw cmajor_sn_volume_macro_1
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_2:
	.db 4, 0, 255
	.dw cmajor_sn_volume_macro_2
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_3:
	.db 12, 0, 255
	.dw cmajor_sn_volume_macro_3
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_4:
	.db 7, 0, 255
	.dw cmajor_sn_volume_macro_4
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_5:
	.db 11, 0, 255
	.dw cmajor_sn_volume_macro_5
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_6:
	.db 0, 0, 0
	.dw 0xffff
	.db 16
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_7:
	.db 0, 0, 0
	.dw 0xffff
	.db 0
	.db 0, 33, 24, 0, 242, 244, 255, 235
cmajor_sn_instrument_8:
	.db 0, 0, 0
	.dw 0xffff
	.db 255
	.db 30, 10, 200, 8, 27, 10, 255, 255
cmajor_sn_instrument_9:
	.db 0, 0, 0
	.dw 0xffff
	.db 255
	.db 111, 0, 80, 5, 192, 1, 255, 255
cmajor_sn_instrument_10:
	.db 0, 0, 0
	.dw 0xffff
	.db 48
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_11:
	.db 0, 0, 0
	.dw 0xffff
	.db 128
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_sn_instrument_12:
	.db 8, 0, 255
	.dw cmajor_sn_volume_macro_6
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255


cmajor_sn_order_pointers:
	.dw cmajor_sn_orders_channel_0
	.dw cmajor_sn_orders_channel_1
	.dw cmajor_sn_orders_channel_2
	.dw cmajor_sn_orders_channel_3
cmajor_sn_orders:
cmajor_sn_orders_channel_0:

	.dw cmajor_sn_patterns_0_0, cmajor_sn_patterns_0_0, cmajor_sn_patterns_0_1, cmajor_sn_patterns_0_0
cmajor_sn_orders_channel_1:

	.dw cmajor_sn_patterns_1_0, cmajor_sn_patterns_1_0, cmajor_sn_patterns_1_0, cmajor_sn_patterns_1_0
cmajor_sn_orders_channel_2:

	.dw cmajor_sn_patterns_2_0, cmajor_sn_patterns_2_0, cmajor_sn_patterns_2_0, cmajor_sn_patterns_2_0
cmajor_sn_orders_channel_3:

	.dw cmajor_sn_patterns_3_0, cmajor_sn_patterns_3_0, cmajor_sn_patterns_3_0, cmajor_sn_patterns_3_0


cmajor_sn_patterns:
cmajor_sn_patterns_0_0:
.db INSTRUMENT_CHANGE, 3, PORTAMENTO, 4, NOTE_ON, 144, 39, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 44, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 41, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 46, LINE_WAIT, 2, NOTE_OFF, 255
cmajor_sn_patterns_0_1:
.db INSTRUMENT_CHANGE, 12, NOTE_ON, 144, 39, 255, 255, NOTE_ON, 144, 44, 255, 255, NOTE_ON, 144, 46, 255, 255, NOTE_ON, 144, 48, 255, 255, NOTE_ON, 144, 50, 255, 255, NOTE_ON, 144, 43, 255, 255, NOTE_ON, 144, 48, 255, 255, NOTE_ON, 144, 46, 255, 255
cmajor_sn_patterns_1_0:
.db INSTRUMENT_CHANGE, 4, NOTE_ON, 176, 15, 255, 255, NOTE_ON, 176, 17, 255, NOTE_ON, 176, 19, 255, NOTE_ON, 176, 20, 255, 255, NOTE_ON, 176, 24, 255, 255, NOTE_ON, 176, 17, 255, 255, NOTE_ON, 176, 19, 255, NOTE_ON, 176, 20, 255, NOTE_ON, 176, 22, 255, 255, NOTE_ON, 176, 26, 255, 255
cmajor_sn_patterns_2_0:
.db LINE_WAIT, 15
cmajor_sn_patterns_3_0:
.db LINE_WAIT, 15
