#include <gbdk/platform.h>
#include "../../music_driver_sdas/banjo.h"
#include "../../music_driver_sdas/banjo_sfx.h"

song_data_t *sfx_ptr;
uint8_t sfx_bank;

// used with autobanking to get the bank numbers for song data
BANKREF_EXTERN(cmajor)
BANKREF_EXTERN(cmajor_sn)
BANKREF_EXTERN(sfx_test_sn)
BANKREF_EXTERN(sfx_test)

// references to song data
extern song_data_t cmajor;
extern song_data_t cmajor_sn;
extern song_data_t sfx_test_sn;
extern song_data_t sfx_test;

// allocate arrays in ram with space for the maximum number of channels
// we'll be using in our songs
BANJO_SONG_CHANNELS(CHAN_COUNT_SN + CHAN_COUNT_OPLL)


void main(void)
{
    uint8_t joy;
    uint8_t joy_last;
    uint8_t joy_pressed;

    joy = joy_last = joy_pressed = 0;

    disable_interrupts();

    // get information about the system
    banjo_check_hardware();

    // we have an OPLL chip
    if (banjo_has_chips & BANJO_HAS_OPLL)
    {
        // initialise banjo specifying
        // * the number of channels we'll need
        // * that we're using an opll chip
        banjo_init(CHAN_COUNT_OPLL, BANJO_HAS_OPLL);

        // set up a pointer to the OPLL sfx
        // and make a note of its bank
        sfx_ptr = (song_data_t *) sfx_test;
        sfx_bank = BANK(sfx_test);

        // change to the song's bank and play the song
        SWITCH_ROM2(BANK(cmajor));
        banjo_play_song(&cmajor);
    }
    // we don't have an OPLL chip, use the SN 
    else
    {
        // initialise banjo specifying
        // * the number of channels we'll need
        // * that we're using the sn chip
        banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);

        // set up a pointer to the SN sfx
        // and make a note of its bank
        sfx_ptr = (song_data_t *) sfx_test_sn;
        sfx_bank = BANK(sfx_test_sn);

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

        // get joypad input
        joy = joypad();
        joy_pressed = (joy ^ joy_last) & joy;
        joy_last = joy;

        // pressing the A button will play sfx
        if (joy_pressed & J_A)
        {
            // check if an sfx is already playing
            // and stop it if it is
            if (sfx_playing)
            {
                // make sure to switch to the sfx's bank before stopping it
                SWITCH_ROM2(sfx_state.bank);
                banjo_sfx_stop();
            }

            // switch to the sfx's bank and play it
            // this will play a different sfx depending on the chip
            // as we set up sfx_ptr and sfx_bank earlier
            SWITCH_ROM2(sfx_bank);
            banjo_play_sfx(sfx_ptr);
        }

        SWITCH_ROM2(sfx_state.bank);
        banjo_update_sfx();

        // wait for the next vsync
        vsync();
    }
}