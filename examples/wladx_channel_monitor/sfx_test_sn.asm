sfx_test_sn:
sfx_test_sn_magic_byte:
.db 186
sfx_test_sn_bank:
.db 0
sfx_test_sn_channel_count:
.db 1
sfx_test_sn_loop:
.db 0
sfx_test_sn_sfx_channel:
.db 2
sfx_test_sn_has_sn:
.db 1
sfx_test_sn_has_fm:
.db 0
sfx_test_sn_has_fm_drums:
.db 0
sfx_test_sn_time_base:
.db 1
sfx_test_sn_speed_1:
.db 5
sfx_test_sn_speed_2:
.db 5
sfx_test_sn_pattern_length:
.db 16
sfx_test_sn_orders_length:
.db 1
sfx_test_sn_instrument_ptrs:
.dw sfx_test_sn_instrument_pointers
sfx_test_sn_order_ptrs:
.dw sfx_test_sn_order_pointers
sfx_test_sn_channel_types:
.db 0, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255


sfx_test_sn_volume_macros:
sfx_test_sn_volume_macro_0:
.db 0, 5, 7, 9, 0, 2, 4, 10, 10, 10
sfx_test_sn_instrument_pointers:
.dw sfx_test_sn_instrument_0


sfx_test_sn_instrument_data:
sfx_test_sn_instrument_0:
	.db 10, 0, 6
	.dw sfx_test_sn_volume_macro_0
	.db 255
	.db 255, 255, 255, 255, 255, 255, 255, 255


sfx_test_sn_order_pointers:
	.dw sfx_test_sn_orders_channel_2
sfx_test_sn_orders:
sfx_test_sn_orders_channel_2:

	.dw sfx_test_sn_patterns_2_0


sfx_test_sn_patterns:
sfx_test_sn_patterns_2_0:
.db INSTRUMENT_CHANGE, 0, ARPEGGIO, 7, 12, NOTE_ON, 208, 55, LINE_WAIT, 2, NOTE_ON, 208, 48, LINE_WAIT, 2, NOTE_ON, 208, 44, LINE_WAIT, 2, NOTE_ON, 208, 41, LINE_WAIT, 2, NOTE_OFF, LINE_WAIT, 3
