#include "banjo.h"

song_data_t const sfx_test_sn = {
	186,
	0,
	1,
	0,
	2,
	1,
	0,
	0,
	1,
	5,
	5,
	16,
	1,
	sfx_test_sn_instrument_pointers,
	sfx_test_sn_order_pointers,
	{0, 2, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 }
};

unsigned char const sfx_test_sn_volume_macro_0[] = {0, 5, 7, 9, 0, 2, 4, 10, 10, 10};

instrument_t const * const sfx_test_sn_instrument_pointers[] = {
	&sfx_test_sn_instrument_0,
};

instrument_t const sfx_test_sn_instrument_0 = {
	10, 0, 6,
	sfx_test_sn_volume_macro_0,
	255,
	{255, 255, 255, 255, 255, 255, 255, 255}
};

unsigned char const * const * const sfx_test_sn_order_pointers[] = {
	sfx_test_sn_orders_channel_2,
};

unsigned char const * const sfx_test_sn_orders_channel_2[] = {
	sfx_test_sn_patterns_2_0,
};


unsigned char const sfx_test_sn_patterns_2_0[] = {
	INSTRUMENT_CHANGE, 0, ARPEGGIO, 7, 12, NOTE_ON, 208, 55, LINE_WAIT, 2, NOTE_ON, 208, 48, LINE_WAIT, 2, NOTE_ON, 208, 44, LINE_WAIT, 2, NOTE_ON, 208, 41, LINE_WAIT, 2, NOTE_OFF, LINE_WAIT, 3
};


