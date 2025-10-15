#include "SMSlib.h"

#include "../../music_driver_sdas/banjo.h"
#include "../../music_driver_sdas/banjo_queue.h"
#include "../../music_driver_sdas/banjo_sfx.h"

#include "song_table.h"

channel_t song_channels[CHAN_COUNT_OPLL_DRUMS];

unsigned char tic;

void main(void)
{
	unsigned int keys;

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));

	/* Turn on the display */
	SMS_displayOn();

	// used to check whether the FM unit is present, and whether we're on a Game Gear in Game Gear mode
	banjo_check_hardware();

	banjo_sfx_init();

	// check whether the FM unit is present
	// use either the FM or SN song/sfx tables defined in song_tables.h depending on the result
	if (banjo_has_chips & BANJO_HAS_OPLL)
	{
		banjo_init(CHAN_COUNT_OPLL_DRUMS, BANJO_HAS_OPLL);
		banjo_set_song_table(song_table_fm);
		banjo_set_sfx_table(sfx_table_fm);
	}
	else
	{
		banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);
		banjo_set_song_table(song_table_sn);
		banjo_set_sfx_table(sfx_table_sn);
	}

	// play song 0
	banjo_queue_song(0);

	tic = 0;

	for(;;)
	{

		SMS_waitForVBlank();

		keys = SMS_getKeysPressed();

		if (keys & 0x20)
		{
			banjo_queue_sfx(0);
		}

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
