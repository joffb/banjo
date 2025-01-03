#include <gbdk/platform.h>
#include "../../music_driver_sdas/banjo.h"

// used with autobanking to get the bank numbers for song data
BANKREF_EXTERN(cmajor)
BANKREF_EXTERN(cmajor_sn)
BANKREF_EXTERN(cmajor_sn_test)


// references to song data
extern song_data_t cmajor;
extern song_data_t cmajor_sn;
extern song_data_t cmajor_sn_test;

// allocate arrays in ram with space for the maximum number of channels
// we'll be using in our songs
BANJO_SONG_CHANNELS(CHAN_COUNT_SN + CHAN_COUNT_OPLL)


void main(void)
{
    disable_interrupts();

    // get information about the system
    banjo_check_hardware();

    // do we have an opll chip?
    if (banjo_has_chips & BANJO_HAS_OPLL)
    {
        // initialise banjo specifying
        // * the number of channels we'll need
        // * that we're using an opll chip
        banjo_init(CHAN_COUNT_OPLL_DRUMS, BANJO_HAS_OPLL);

        // change to the song's bank and play the song
        SWITCH_ROM2(BANK(cmajor));
        banjo_play_song(&cmajor);
    }
    else
    {
        // initialise banjo specifying
        // * the number of channels we'll need
        // * that we're using an opll chip
        banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);

        // change to the song's bank and play the song
        SWITCH_ROM2(BANK(cmajor_sn));
        banjo_play_song(&cmajor_sn);
    }

    enable_interrupts();

    while (1)
    {
        // song_state.bank should have the song's bank number 
        // switch to the song's bank and update the song
        SWITCH_ROM2(song_state.bank);
        banjo_update_song();

        // wait for the next vsync
        vsync();
    }
}