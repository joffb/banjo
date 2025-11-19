
; banjo sound driver
; Joe Kennedy 2025

; ix: channel
; iy: music state
banjo_init_opm:

    ld b, 8
    ld de, _sizeof_channel

    music_init_opm_loop:

        ; default volume
        ld (ix + channel.volume), 0x7f

        ; subchannel number
        ld a, 8
        sub a, b
        ld (ix + channel.subchannel), a

        ; move to next channel
        add ix, de

        djnz music_init_opm_loop

    ret