#include "banjo.h"

song_data_t const cmajor = {
	186,
	0,
	9,
	1,
	255,
	0,
	1,
	0,
	2,
	6,
	6,
	16,
	2,
	cmajor_instrument_pointers,
	cmajor_order_pointers,
	{1, 0, 1, 1, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 }
};

unsigned char const cmajor_volume_macro_0[] = {0, 0, 0, 1, 5, 8, 13, 15, 15};
unsigned char const cmajor_volume_macro_1[] = {0, 0, 0, 2, 6, 8, 10, 11, 13, 13, 14, 15, 15, 15, 15};
unsigned char const cmajor_volume_macro_2[] = {2, 7, 13, 15};
unsigned char const cmajor_volume_macro_3[] = {13, 11, 8, 7, 6, 5, 4, 5, 6, 7, 8, 9};
unsigned char const cmajor_volume_macro_4[] = {3, 5, 6, 7, 8, 9, 10, 11, 13, 15, 15, 15};
unsigned char const cmajor_volume_macro_5[] = {2, 5, 3, 8, 5, 11, 7, 13, 10, 14, 15};

instrument_t const * const cmajor_instrument_pointers[] = {
	&cmajor_instrument_0,
	&cmajor_instrument_1,
	&cmajor_instrument_2,
	&cmajor_instrument_3,
	&cmajor_instrument_4,
	&cmajor_instrument_5,
	&cmajor_instrument_6,
	&cmajor_instrument_7,
	&cmajor_instrument_8,
	&cmajor_instrument_9,
	&cmajor_instrument_10,
	&cmajor_instrument_11,
};

instrument_t const cmajor_instrument_0 = {
	9, 0, 255,
	cmajor_volume_macro_0,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_1 = {
	15, 0, 255,
	cmajor_volume_macro_1,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_2 = {
	4, 0, 255,
	cmajor_volume_macro_2,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_3 = {
	12, 0, 255,
	cmajor_volume_macro_3,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_4 = {
	12, 0, 255,
	cmajor_volume_macro_4,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_5 = {
	11, 0, 255,
	cmajor_volume_macro_5,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_6 = {
	0, 0, 0,
	0xffff,
	16,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_7 = {
	0, 0, 0,
	0xffff,
	0,
	{0, 33, 24, 0, 242, 244, 255, 235}
};

instrument_t const cmajor_instrument_8 = {
	0, 0, 0,
	0xffff,
	255,
	{30, 10, 200, 8, 27, 10, 255, 255}
};

instrument_t const cmajor_instrument_9 = {
	0, 0, 0,
	0xffff,
	255,
	{111, 0, 80, 5, 192, 1, 255, 255}
};

instrument_t const cmajor_instrument_10 = {
	0, 0, 0,
	0xffff,
	48,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

instrument_t const cmajor_instrument_11 = {
	0, 0, 0,
	0xffff,
	128,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

unsigned char const * const * const cmajor_order_pointers[] = {
	cmajor_orders_channel_0,
	cmajor_orders_channel_1,
	cmajor_orders_channel_2,
	cmajor_orders_channel_3,
	cmajor_orders_channel_4,
	cmajor_orders_channel_5,
	cmajor_orders_channel_6,
	cmajor_orders_channel_7,
	cmajor_orders_channel_8,
};

unsigned char const * const cmajor_orders_channel_0[] = {
	cmajor_patterns_0_0,
	cmajor_patterns_0_0,
};
unsigned char const * const cmajor_orders_channel_1[] = {
	cmajor_patterns_1_0,
	cmajor_patterns_1_0,
};
unsigned char const * const cmajor_orders_channel_2[] = {
	cmajor_patterns_2_0,
	cmajor_patterns_2_0,
};
unsigned char const * const cmajor_orders_channel_3[] = {
	cmajor_patterns_3_0,
	cmajor_patterns_3_0,
};
unsigned char const * const cmajor_orders_channel_4[] = {
	cmajor_patterns_4_0,
	cmajor_patterns_4_0,
};
unsigned char const * const cmajor_orders_channel_5[] = {
	cmajor_patterns_5_0,
	cmajor_patterns_5_0,
};
unsigned char const * const cmajor_orders_channel_6[] = {
	cmajor_patterns_6_0,
	cmajor_patterns_6_0,
};
unsigned char const * const cmajor_orders_channel_7[] = {
	cmajor_patterns_7_0,
	cmajor_patterns_7_0,
};
unsigned char const * const cmajor_orders_channel_8[] = {
	cmajor_patterns_8_0,
	cmajor_patterns_8_0,
};


unsigned char const cmajor_patterns_0_0[] = {
	INSTRUMENT_CHANGE, 10, PORTAMENTO, 4, NOTE_ON, 0, 48, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 53, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 50, LINE_WAIT, 2, NOTE_OFF, 255, NOTE_ON, 0, 55, LINE_WAIT, 2, NOTE_OFF, 255
};
unsigned char const cmajor_patterns_1_0[] = {
	INSTRUMENT_CHANGE, 7, NOTE_ON, 0, 24, 255, 255, NOTE_ON, 0, 26, 255, NOTE_ON, 0, 28, 255, NOTE_ON, 0, 29, 255, 255, NOTE_ON, 0, 33, 255, 255, NOTE_ON, 0, 26, 255, 255, NOTE_ON, 0, 28, 255, NOTE_ON, 0, 29, 255, NOTE_ON, 0, 31, 255, 255, NOTE_ON, 0, 35, 255, 255
};
unsigned char const cmajor_patterns_2_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_3_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_4_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_5_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_6_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_7_0[] = {
	LINE_WAIT, 15
};
unsigned char const cmajor_patterns_8_0[] = {
	LINE_WAIT, 15
};


