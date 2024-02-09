cmajor:
cmajor_magic_byte:
.db 186
cmajor_bank:
.db 0
cmajor_channel_count:
.db 9
cmajor_loop:
.db 1
cmajor_sfx_channel:
.db 255
cmajor_has_sn:
.db 0
cmajor_has_fm:
.db 1
cmajor_has_fm_drums:
.db 0
cmajor_time_base:
.db 2
cmajor_speed_1:
.db 6
cmajor_speed_2:
.db 6
cmajor_pattern_length:
.db 16
cmajor_orders_length:
.db 2
cmajor_instrument_ptrs:
.dw cmajor_instrument_pointers
cmajor_order_ptrs:
.dw cmajor_order_pointers
cmajor_channel_types:
.db 1, 0, 1, 1, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255


cmajor_volume_macros:
cmajor_volume_macro_0:
.db 0, 0, 0, 1, 5, 8, 13, 15, 15
cmajor_volume_macro_1:
.db 0, 0, 0, 2, 6, 8, 10, 11, 13, 13, 14, 15, 15, 15, 15
cmajor_volume_macro_2:
.db 2, 7, 13, 15
cmajor_volume_macro_3:
.db 13, 11, 8, 7, 6, 5, 4, 5, 6, 7, 8, 9
cmajor_volume_macro_4:
.db 3, 5, 6, 7, 8, 9, 10, 11, 13, 15, 15, 15
cmajor_volume_macro_5:
.db 2, 5, 3, 8, 5, 11, 7, 13, 10, 14, 15
cmajor_instrument_pointers:
.dw cmajor_instrument_0
.dw cmajor_instrument_1
.dw cmajor_instrument_2
.dw cmajor_instrument_3
.dw cmajor_instrument_4
.dw cmajor_instrument_5
.dw cmajor_instrument_6
.dw cmajor_instrument_7
.dw cmajor_instrument_8
.dw cmajor_instrument_9
.dw cmajor_instrument_10
.dw cmajor_instrument_11


cmajor_instrument_data:
cmajor_instrument_0:
	.db 9, 0, 255
	.dw cmajor_volume_macro_0
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_1:
	.db 15, 0, 255
	.dw cmajor_volume_macro_1
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_2:
	.db 4, 0, 255
	.dw cmajor_volume_macro_2
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_3:
	.db 12, 0, 255
	.dw cmajor_volume_macro_3
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_4:
	.db 12, 0, 255
	.dw cmajor_volume_macro_4
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_5:
	.db 11, 0, 255
	.dw cmajor_volume_macro_5
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_6:
	.db 0, 0, 0
	.dw 0xffff
	.db 16
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_7:
	.db 0, 0, 0
	.dw 0xffff
	.db 0
	.db 0, 33, 24, 0, 242, 244, 255, 235
cmajor_instrument_8:
	.db 0, 0, 0
	.dw 0xffff
	.db 255
	.db 30, 10, 200, 8, 27, 10, 255, 255
cmajor_instrument_9:
	.db 0, 0, 0
	.dw 0xffff
	.db 255
	.db 111, 0, 80, 5, 192, 1, 255, 255
cmajor_instrument_10:
	.db 0, 0, 0
	.dw 0xffff
	.db 48
	.db 255, 255, 255, 255, 255, 255, 255, 255
cmajor_instrument_11:
	.db 0, 0, 0
	.dw 0xffff
	.db 128
	.db 255, 255, 255, 255, 255, 255, 255, 255


cmajor_order_pointers:
	.dw cmajor_orders_channel_0
	.dw cmajor_orders_channel_1
	.dw cmajor_orders_channel_2
	.dw cmajor_orders_channel_3
	.dw cmajor_orders_channel_4
	.dw cmajor_orders_channel_5
	.dw cmajor_orders_channel_6
	.dw cmajor_orders_channel_7
	.dw cmajor_orders_channel_8
cmajor_orders:
cmajor_orders_channel_0:

	.dw cmajor_patterns_0_0, cmajor_patterns_0_0
cmajor_orders_channel_1:

	.dw cmajor_patterns_1_0, cmajor_patterns_1_0
cmajor_orders_channel_2:

	.dw cmajor_patterns_2_0, cmajor_patterns_2_0
cmajor_orders_channel_3:

	.dw cmajor_patterns_3_0, cmajor_patterns_3_0
cmajor_orders_channel_4:

	.dw cmajor_patterns_4_0, cmajor_patterns_4_0
cmajor_orders_channel_5:

	.dw cmajor_patterns_5_0, cmajor_patterns_5_0
cmajor_orders_channel_6:

	.dw cmajor_patterns_6_0, cmajor_patterns_6_0
cmajor_orders_channel_7:

	.dw cmajor_patterns_7_0, cmajor_patterns_7_0
cmajor_orders_channel_8:

	.dw cmajor_patterns_8_0, cmajor_patterns_8_0


cmajor_patterns:
cmajor_patterns_0_0:
.db INSTRUMENT_CHANGE, 10, PORTAMENTO, 4, NOTE_ON, 0, 48, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 53, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 50, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 55, LINE_WAIT, 2, NOTE_OFF, 255
cmajor_patterns_1_0:
.db INSTRUMENT_CHANGE, 7, NOTE_ON, 0, 24, 255, 255, NOTE_ON, 0, 26, 255, NOTE_ON, 0, 28, 255, NOTE_ON, 0, 29, 255, 255, NOTE_ON, 0, 33, 255, 255, NOTE_ON, 0, 26, 255, 255, NOTE_ON, 0, 28, 255, NOTE_ON, 0, 29, 255, NOTE_ON, 0, 31, 255, 255, NOTE_ON, 0, 35, 255, 255
cmajor_patterns_2_0:
.db LINE_WAIT, 15
cmajor_patterns_3_0:
.db LINE_WAIT, 15
cmajor_patterns_4_0:
.db LINE_WAIT, 15
cmajor_patterns_5_0:
.db LINE_WAIT, 15
cmajor_patterns_6_0:
.db LINE_WAIT, 15
cmajor_patterns_7_0:
.db LINE_WAIT, 15
cmajor_patterns_8_0:
.db LINE_WAIT, 15
