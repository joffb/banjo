#include "banjo.h"

song_data_t const sfx_test = {
	186,
	0,
	1,
	0,
	2,
	0,
	1,
	0,
	1,
	5,
	5,
	16,
	1,
	sfx_test_instrument_pointers,
	sfx_test_order_pointers,
	{1, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 }
};

unsigned char const sfx_test_volume_macro_0[] = {0, 5, 7, 9, 0, 2, 4, 10, 10, 10};

instrument_t const * const sfx_test_instrument_pointers[] = {
	&sfx_test_instrument_0,
};

instrument_t const sfx_test_instrument_0 = {
	10, 0, 6,
	sfx_test_volume_macro_0,
	128,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

unsigned char const * const * const sfx_test_order_pointers[] = {
	sfx_test_orders_channel_2,
};

unsigned char const * const sfx_test_orders_channel_2[] = {
	sfx_test_patterns_2_0,
};


unsigned char const sfx_test_patterns_2_0[] = {
	INSTRUMENT_CHANGE, 0, ARPEGGIO, 7, 12, NOTE_ON, 0, 64, LINE_WAIT, 2, NOTE_ON, 0, 57, LINE_WAIT, 2, NOTE_ON, 0, 53, LINE_WAIT, 2, NOTE_ON, 0, 50, LINE_WAIT, 2, NOTE_OFF, LINE_WAIT, 3
};


