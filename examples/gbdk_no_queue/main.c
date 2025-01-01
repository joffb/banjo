#include <gbdk/platform.h>
#include "../../music_driver_sdas/banjo.h"

BANJO_SONG_CHANNELS(CHAN_COUNT_SN + CHAN_COUNT_OPLL)

BANKREF_EXTERN(cmajor)
BANKREF_EXTERN(cmajor_sn)

extern song_data_t cmajor;
extern song_data_t cmajor_sn;

void main(void)
{
    disable_interrupts();

    banjo_check_hardware();

    if (banjo_has_chips & BANJO_HAS_OPLL)
    {
        banjo_init(CHAN_COUNT_OPLL_DRUMS, BANJO_HAS_OPLL);

        SWITCH_ROM2(BANK(cmajor));
        banjo_play_song(&cmajor);
        song_state.bank = BANK(cmajor);
    }
    else
    {
        banjo_init(CHAN_COUNT_SN, BANJO_HAS_SN);

        SWITCH_ROM2(BANK(cmajor_sn));
        banjo_play_song(&cmajor_sn);
        song_state.bank = BANK(cmajor_sn);
    }

    enable_interrupts();

    while (1)
    {
        SWITCH_ROM2(song_state.bank);
        banjo_update_song();
        vsync();
    }
}