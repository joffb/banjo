# Queue Library

This is a library currently only intended for the Sega Master System, as the banking calls are hard-coded.

### Song/sfx Tables

The sound driver uses tables of pointers to Song/SFX data.
e.g. in C:
```c
    // References to Songs in other .c files which will be matched up at link time
    extern song_data_t const title_screen_song;
    extern song_data_t const town_song;

    // Song table
    // SONG_DEF(SONG_LABEL, SONG_BANK)
    song_t const song_table[] = {
        SONG_DEF(title_screen_song, 2),
        SONG_DEF(town_song, 2),
    };
```

The locations of these tables should be set with the `banjo_set_song_table(song_table_ptr);` and `banjo_set_sfx_table(sfx_table_ptr);` functions. Depending on what `banjo_check_hardware`found you can decide which table to load (e.g. if no FM unit is present, use the SN song and sfx tables instead of the FM ones).

To queue up a song or sfx, you use `banjo_queue_song(song);` or `banjo_queue_song(sfx);`. The values passed to these should be the index of the song in the table. Using the above table in the C code as an example, `banjo_queue_song(0);` would queue up the title_screen_song, `banjo_queue_song(1);` the town_song.


If your software supports multiple hardware configurations, it would be best to have a Song and SFX table per hardware configuration.

The advantage of having separate tables for different hardware is that you could do something like this:

```c
    // do we have an fm unit? use the _fm table
    if (banjo_fm_unit_present)
    {
        banjo_set_song_table(song_table_fm);
    }
    // otherwise use the _sn table
    else
    {
        banjo_set_song_table(song_table_sn);
    }

    // queue title screen song
    banjo_queue_song(0);
```

If for example both tables are set up so that song 0 is the title screen song (arranged specifically for each hardware configuration) you only need to set the tables once - all subsequent `banjo_queue_` calls can be exactly the same.

The `banjo_queue_` functions don't immediately start playback of the new SFX or Song - it happens in `banjo_update()`:

### banjo_update()

`banjo_update();` (or `call banjo_update`) should be run at every vblank. It's better to run it *after* any code which updates the VDP, to give that code as much time in vblank as possible.

It's in this function that the queues are checked and playback of a new Song or SFX is started. For SFX, there is an added check of the SFX priority - SFX with lower priority won't start playing if an SFX with higher priority is already playing. If a Song or SFX isn't queued, it updates the ones which are currently playing.

`banjo_update` will also change the bank for slot 2 (0x8000 - 0xbfff) so if you're relying on something being in bank 2 you'll have to change the bank afterwards.

## Function Calls

```c

// handle song/sfx queues and update playing song/sfx
// this will change the bank for slot 2 (i.e. 0x8000 to 0xbfff (writes to mapper at 0xffff))
void banjo_update(void);

// queue song/sfx to be played back starting on next banjo_update
void banjo_queue_song(unsigned char song);
void banjo_queue_sfx(unsigned char sfx);

// set the loop mode for the song/sfx
// by default songs loop, but sfx don't
void banjo_queue_song_loop_mode(unsigned char loop);
void banjo_queue_sfx_loop_mode(unsigned char loop);

// set up pointers to the song and sfx tables
void banjo_set_song_table(const song_t *song_table_ptr);
void banjo_set_sfx_table(const song_t *sfx_table_ptr);

```