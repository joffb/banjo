#include "SMSlib.h"

#include "../../music_driver_sdas/banjo.h"
#include "../../music_driver_sdas/banjo_sfx.h"

#include "song_table.h"

unsigned char tic;

channel_t song_channels[CHAN_COUNT_OPLL_DRUMS];
channel_t *song_channel_ptrs[CHAN_COUNT_OPLL_DRUMS];

void main(void)
{
	unsigned int keys;

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));

	/* Turn on the display */
	SMS_displayOn();

	// used to check whether the FM unit is present, and whether we're on a Game Gear in Game Gear mode
	banjo_check_hardware();

	if (banjo_has_chips & BANJO_HAS_OPLL)
	{
		banjo_init(CHAN_COUNT_OPLL_DRUMS, BANJO_HAS_OPLL);

		SMS_mapROMBank(2);
		banjo_play_song(&cmajor);
	}
	else
	{
		banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);

		SMS_mapROMBank(3);
		banjo_play_song(&cmajor_sn);
	}

	banjo_sfx_init();

	tic = 0;

	for(;;)
	{

		SMS_waitForVBlank();

		keys = SMS_getKeysPressed();

		// play sfx
		if (keys & 0x20)
		{
			SMS_mapROMBank(3);
			banjo_play_sfx(&sfx_test_sn);
		}
		
		if (keys & 0x01)
		{
			banjo_song_fade_out(1);
		}

		if (keys & 0x02)
		{
			banjo_song_fade_in(1);
		}

		if (keys & 0x04)
		{
			SMS_mapROMBank(song_state.bank);
			banjo_song_stop();
		}

		if (keys & 0x08)
		{
			SMS_mapROMBank(song_state.bank);
			banjo_song_resume();
		}

		SMS_setBackdropColor(1);

		SMS_mapROMBank(song_state.bank);
		banjo_update_song();

		SMS_setBackdropColor(0);

		SMS_mapROMBank(sfx_state.bank);
		banjo_update_sfx();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
