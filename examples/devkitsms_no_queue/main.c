#include "SMSlib.h"

#include "banjo.h"
#include "song_table.h"

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

	banjo_init(MODE_SN);

	SMS_mapROMBank(3);
	banjo_play_song(&cmajor_sn, 0xff);

	tic = 0;

	for(;;)
	{

		SMS_waitForVBlank();

		keys = SMS_getKeysPressed();

		if (keys & 0x20)
		{
			SMS_mapROMBank(3);
			banjo_play_sfx(&sfx_test_sn, 0xff);
		}

		SMS_mapROMBank(3);
		banjo_update_song();

		SMS_mapROMBank(3);
		banjo_update_sfx();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
