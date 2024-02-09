sfx_test:
sfx_test_magic_byte:
.db 186
sfx_test_bank:
.db 0
sfx_test_channel_count:
.db 1
sfx_test_loop:
.db 0
sfx_test_sfx_channel:
.db 2
sfx_test_has_sn:
.db 0
sfx_test_has_fm:
.db 1
sfx_test_has_fm_drums:
.db 0
sfx_test_time_base:
.db 1
sfx_test_speed_1:
.db 5
sfx_test_speed_2:
.db 5
sfx_test_pattern_length:
.db 16
sfx_test_orders_length:
.db 1
sfx_test_instrument_ptrs:
.dw sfx_test_instrument_pointers
sfx_test_order_ptrs:
.dw sfx_test_order_pointers
sfx_test_channel_types:
.db 1, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255


sfx_test_volume_macros:
sfx_test_volume_macro_0:
.db 0, 5, 7, 9, 0, 2, 4, 10, 10, 10
sfx_test_instrument_pointers:
.dw sfx_test_instrument_0


sfx_test_instrument_data:
sfx_test_instrument_0:
	.db 10, 0, 6
	.dw sfx_test_volume_macro_0
	.db 128
	.db 255, 255, 255, 255, 255, 255, 255, 255


sfx_test_order_pointers:
	.dw sfx_test_orders_channel_2
sfx_test_orders:
sfx_test_orders_channel_2:

	.dw sfx_test_patterns_2_0


sfx_test_patterns:
sfx_test_patterns_2_0:
.db INSTRUMENT_CHANGE, 0, ARPEGGIO, 7, 12, NOTE_ON, 0, 64, LINE_WAIT, 2, NOTE_ON, 0, 57, LINE_WAIT, 2, NOTE_ON, 0, 53, LINE_WAIT, 2, NOTE_ON, 0, 50, LINE_WAIT, 2, NOTE_OFF, LINE_WAIT, 3
