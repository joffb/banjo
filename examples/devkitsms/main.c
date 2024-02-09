#include "SMSlib.h"

#include "banjo.h"
#include "song_table.h"

// set up song channels
// todo: create struct for channels
#define CHANNEL_COUNT 9
unsigned char song_channels[CHANNEL_COUNT * 32];
unsigned char song_channel_ptrs[CHANNEL_COUNT * 2];

unsigned char tic;

void main(void)
{

	SMS_VRAMmemsetW(0, 0x0000, 16384);

	SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));

	/* Turn on the display */
	SMS_displayOn();

	banjo_init(CHANNEL_COUNT);

	if (banjo_fm_unit_present())
	{
		banjo_queue_song(0);
	}
	else
	{
		banjo_queue_song(1);
	}

	tic = 0;

	/* Do nothing */
	for(;;) {
		SMS_waitForVBlank();

		if (tic == 64)
		{
			if (banjo_fm_unit_present())
			{
				banjo_queue_sfx(0);
			}
			else
			{
				banjo_queue_sfx(1);
			}
		}

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
