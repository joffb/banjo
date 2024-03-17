#include "SMSlib.h"

#include "banjo.h"
#include "song_table.h"

unsigned char tic;

void main(void)
{

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));

	/* Turn on the display */
	SMS_displayOn();

	// used to check whether the FM unit is present, and whether we're on a Game Gear in Game Gear mode
	banjo_check_hardware();

	// check whether the FM unit is present
	// use either the FM or SN song/sfx tables defined in song_tables.h depending on the result
	if (banjo_fm_unit_present)
	{
		banjo_init(MODE_FM);
		banjo_set_song_table(song_table_fm);
		banjo_set_sfx_table(sfx_table_fm);
	}
	else
	{
		banjo_init(MODE_SN);
		banjo_set_song_table(song_table_sn);
		banjo_set_sfx_table(sfx_table_sn);
	}

	// play song 0
	banjo_queue_song(0);

	tic = 0;

	for(;;)
	{

		SMS_waitForVBlank();

		// play an sfx every 256 tics
		if (tic == 64)
		{
			//banjo_queue_sfx(0);
		}
		else if (tic == 80)
		{
			//banjo_sfx_stop();
		}

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
