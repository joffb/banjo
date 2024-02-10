#include "SMSlib.h"

#include "banjo.h"
#include "song_table.h"

// set up song channels
// you should use the BANJO_ALLOCATE_CHANNELS_ macro which allocates the maximum number of channels you'll need
// in this case BANJO_ALLOCATE_CHANNELS_MODE_FM is used as the music uses either 9 channels of FM or 4 channels of SN
BANJO_ALLOCATE_CHANNELS_MODE_FM()

unsigned char tic;

void main(void)
{

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));

	/* Turn on the display */
	SMS_displayOn();

	// check whether the FM unit is present
	// use either the FM or SN song/sfx tables defined in song_tables.h depending on the result
	if (banjo_check_fm_unit_present())
	{
		banjo_init(MODE_FM);
		song_table_ptr = song_table_fm;
		sfx_table_ptr = sfx_table_fm;
	}
	else
	{
		banjo_init(MODE_SN);
		song_table_ptr = song_table_sn;
		sfx_table_ptr = sfx_table_sn;
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
			banjo_queue_sfx(0);
		}

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
