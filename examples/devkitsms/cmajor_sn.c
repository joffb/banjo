#include "banjo.h"

song_data_t const cmajor_sn = {
	186,
	0,
	4,
	1,
	255,
	1,
	0,
	0,
	2,
	6,
	6,
	16,
	4,
	cmajor_sn_instrument_pointers,
	cmajor_sn_order_pointers,
	{0, 0, 0, 1, 0, 2, 0, 3, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 }
};

unsigned char const cmajor_sn_volume_macro_0[] = {0, 0, 0, 1, 5, 8, 13, 15, 15};
unsigned char const cmajor_sn_volume_macro_1[] = {0, 0, 0, 2, 6, 8, 10, 11, 13, 13, 14, 15, 15, 15, 15};
unsigned char const cmajor_sn_volume_macro_2[] = {2, 7, 13, 15};
unsigned char const cmajor_sn_volume_macro_3[] = {10, 9, 8, 7, 6, 5, 4, 5, 6, 7, 8, 9};
unsigned char const cmajor_sn_volume_macro_4[] = {3, 5, 6, 7, 8, 11, 15};
unsigned char const cmajor_sn_volume_macro_5[] = {2, 5, 3, 8, 5, 11, 7, 13, 10, 14, 15};
unsigned char const cmajor_sn_volume_macro_6[] = {1, 5, 9, 10, 12, 13, 14, 14};

instrument_t const * const cmajor_sn_instrument_pointers[] = {
	&cmajor_sn_instrument_0,
	&cmajor_sn_instrument_1,
	&cmajor_sn_instrument_2,
	&cmajor_sn_instrument_3,
	&cmajor_sn_instrument_4,
	&cmajor_sn_instrument_5,
	&cmajor_sn_instrument_6,
	&cmajor_sn_instrument_7,
	&cmajor_sn_instrument_8,
	&cmajor_sn_instrument_9,
	&cmajor_sn_instrument_10,
	&cmajor_sn_instrument_11,
	&cmajor_sn_instrument_12,
};

instrument_t const cmajor_sn_instrument_0 = {
	9, 0, 4,
	cmajor_sn_volume_macro_0,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_1 = {
	15, 0, 255,
	cmajor_sn_volume_macro_1,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_2 = {
	4, 0, 255,
	cmajor_sn_volume_macro_2,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_3 = {
	12, 0, 255,
	cmajor_sn_volume_macro_3,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_4 = {
	7, 0, 255,
	cmajor_sn_volume_macro_4,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_5 = {
	11, 0, 255,
	cmajor_sn_volume_macro_5,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_6 = {
	0, 0, 0,
	0xffff,
	16,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_7 = {
	0, 0, 0,
	0xffff,
	0,
	{0, 33, 24, 0, 242, 244, 255, 235}
};

instrument_t const cmajor_sn_instrument_8 = {
	0, 0, 0,
	0xffff,
	255,
	{30, 10, 200, 8, 27, 10, 255, 255}
};

instrument_t const cmajor_sn_instrument_9 = {
	0, 0, 0,
	0xffff,
	255,
	{111, 0, 80, 5, 192, 1, 255, 255}
};

instrument_t const cmajor_sn_instrument_10 = {
	0, 0, 0,
	0xffff,
	48,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_11 = {
	0, 0, 0,
	0xffff,
	128,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_sn_instrument_12 = {
	8, 0, 255,
	cmajor_sn_volume_macro_6,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

unsigned char const * const * const cmajor_sn_order_pointers[] = {
	cmajor_sn_orders_channel_0,
	cmajor_sn_orders_channel_1,
	cmajor_sn_orders_channel_2,
	cmajor_sn_orders_channel_3,
};

unsigned char const * const cmajor_sn_orders_channel_0[] = {
	cmajor_sn_patterns_0_0,
	cmajor_sn_patterns_0_0,
	cmajor_sn_patterns_0_1,
	cmajor_sn_patterns_0_0,
};
unsigned char const * const cmajor_sn_orders_channel_1[] = {
	cmajor_sn_patterns_1_0,
	cmajor_sn_patterns_1_0,
	cmajor_sn_patterns_1_0,
	cmajor_sn_patterns_1_0,
};
unsigned char const * const cmajor_sn_orders_channel_2[] = {
	cmajor_sn_patterns_2_0,
	cmajor_sn_patterns_2_0,
	cmajor_sn_patterns_2_0,
	cmajor_sn_patterns_2_0,
};
unsigned char const * const cmajor_sn_orders_channel_3[] = {
	cmajor_sn_patterns_3_0,
	cmajor_sn_patterns_3_0,
	cmajor_sn_patterns_3_0,
	cmajor_sn_patterns_3_0,
};


unsigned char const cmajor_sn_patterns_0_0[] = {
	INSTRUMENT_CHANGE, 3, PORTAMENTO, 4, NOTE_ON, 144, 39, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 44, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 41, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 144, 46, LINE_WAIT, 2, NOTE_OFF, 255
};
unsigned char const cmajor_sn_patterns_0_1[] = {
	INSTRUMENT_CHANGE, 12, NOTE_ON, 144, 39, 255, 255, NOTE_ON, 144, 44, 255, 255, NOTE_ON, 144, 46, 255, 255, NOTE_ON, 144, 48, 255, 255, NOTE_ON, 144, 50, 255, 255, NOTE_ON, 144, 43, 255, 255, NOTE_ON, 144, 48, 255, 255, NOTE_ON, 144, 46, 255, 255
};
unsigned char const cmajor_sn_patterns_1_0[] = {
	INSTRUMENT_CHANGE, 4, NOTE_ON, 176, 15, 255, 255, NOTE_ON, 176, 17, 255, NOTE_ON, 176, 19, 255, NOTE_ON, 176, 20, 255, 255, NOTE_ON, 176, 24, 255, 255, NOTE_ON, 176, 17, 255, 255, NOTE_ON, 176, 19, 255, NOTE_ON, 176, 20, 255, NOTE_ON, 176, 22, 255, 255, NOTE_ON, 176, 26, 255, 255
};
unsigned char const cmajor_sn_patterns_2_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_sn_patterns_3_0[] = {
	LINE_WAIT, 15
};


