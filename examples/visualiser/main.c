#include "SMSlib.h"

#include "banjo.h"
#include "banjo_sfx.h"
#include "banjo_queue.h"

#include "song_table.h"

#include "bank2.h"
#include "key_x_tile.h"

BANJO_SONG_CHANNELS(CHAN_COUNT_OPLL_DRUMS)

unsigned char tic;
const unsigned char channel_lit_tiles[16] = {0, 8, 16, 24, 32, 40, 48, 0, 8, 16, 24, 32, 40, 48, 0, 8};

void draw_logo(unsigned char tile_x, unsigned char tile_y)
{
	unsigned char i;
	
	// draw top row of piano key tiles
	SMS_setNextTileatXY(tile_x, tile_y);

	for (i = 0; i < 6; i++)
	{
		SMS_setTile(i + 32);
	}

	// draw bottom row of piano key tiles
	SMS_setNextTileatXY(tile_x, tile_y + 1);

	for (i = 0; i < 6; i++)
	{
		SMS_setTile(i + 32 + 6);
	}
}

void draw_keyboard(unsigned char tile_y, unsigned char type)
{
	unsigned char i;
	type = (type * 2) + 4;
	
	// draw top row of piano key tiles
	SMS_setNextTileatXY(0, tile_y);

	SMS_setTile(1);
	SMS_setTile(type);
	SMS_setTile(type + 1);

	for (i = 0; i < 28; i++)
	{
		SMS_setTile(2);
	}

	SMS_setTile(3);

	// draw top row of piano key tiles
	SMS_setNextTileatXY(0, tile_y + 1);

	for (i = 0; i < 32; i++)
	{
		SMS_setTile((i % 7) + 16);
	}

	// draw bottom row of piano key tiles
	SMS_setNextTileatXY(0, tile_y + 2);

	for (i = 0; i < 32; i++)
	{
		SMS_setTile((i % 7) + 16 + 7);
	}
}

void main(void)
{
	channel_t *channel;

	unsigned char i, keyboard_y, key_lit_tile;

	SMS_VRAMmemsetW(0, 0x0000, 16384);
	SMS_setSpriteMode(1);
	
	SMS_initSprites();

	SMS_loadTiles(labels_tiles_bin, 1, labels_tiles_bin_size);
	SMS_loadTiles(banjo_tiles_bin, 32, banjo_tiles_bin_size);
	SMS_loadTiles(keyboard_tiles_bin, 16, keyboard_tiles_bin_size);
	SMS_loadTiles(keys_lit_tiles_bin, 256, keys_lit_tiles_bin_size);
	SMS_loadBGPalette(palette_bin);
	SMS_loadSpritePalette(sprite_palette_bin);

	draw_keyboard(4, 0);
	draw_keyboard(8, 1);
	draw_keyboard(12, 2);

	draw_logo(1, 1);

	/* Turn on the display */
	SMS_displayOn();

	// used to check whether the FM unit is present, and whether we're on a Game Gear in Game Gear mode
	banjo_check_hardware();

	banjo_queue_init();
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

		SMS_initSprites();

		// check we're playing a valid song
		if (song_state.magic_byte == 0xBA)
		{
			// go through all song channels
			for (i = 0; i < song_state.channel_count; i++)
			{
				channel = &song_channels[i];

				// does the channel have a note-on?
				if (channel->flags & CHAN_FLAG_NOTE_ON)
				{
					key_lit_tile = channel_lit_tiles[i];

					if (channel->port == 0x7f)
					{
						keyboard_y = 40;
					}
					else
					{
						if (channel->subchannel < 6)
						{
							keyboard_y = 40 + 32;
						}
						else
						{
							keyboard_y = 40 + 64;
						}
					}

					// draw key sprite for this note-on
					SMS_addSprite_f(keyboard_y, key_x_tile[channel->midi_note] + key_lit_tile);
				}
			}
		}

		SMS_copySpritestoSAT();

		banjo_update();

		tic++;
	}
}

SMS_EMBED_SEGA_ROM_HEADER_16KB(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE_16KB(1,0,"joe k  ","banjo test","");
