#include "SMSlib.h"

#include "banjo.h"
#include "song_table.h"

#include "bank2.h"
#include "key_offsets_tiles.h"

unsigned char tic;

void main(void)
{
	channel_t *channel;
	const offset_tile_t *offset_tile;

	unsigned char i, j, chan_count;
	unsigned int keys;

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setSpriteMode(1);
	
	SMS_initSprites();

	SMS_loadTiles(keyboard_tiles_bin, 16, keyboard_tiles_bin_size);
	SMS_loadTiles(keys_lit_tiles_bin, 256, keys_lit_tiles_bin_size);
	SMS_loadBGPalette(palette_bin);
	SMS_loadSpritePalette(palette_bin);

	SMS_setNextTileatXY(0, 1);

	// draw piano keys
	for (i = 0; i < 32; i++)
	{
		SMS_setTile((i % 7) + 16);
	}

	SMS_setNextTileatXY(0, 2);

	// draw piano keys
	for (i = 0; i < 32; i++)
	{
		SMS_setTile((i % 7) + 16 + 7);
	}

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

		SMS_initSprites();

		if (song_state.magic_byte == 0xBA)
		{
			chan_count = song_state.channel_count;

			for (i = 0; i < chan_count; i++)
			{
				channel = &song_channels[i];
				offset_tile = &key_offsets_tiles[channel->midi_note];

				SMS_addSprite(offset_tile->x, 8, offset_tile->tile);
			}
		}

		SMS_copySpritestoSAT();

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
