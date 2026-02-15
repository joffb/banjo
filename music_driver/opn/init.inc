
; banjo sound driver
; Joe Kennedy 2024

; ix: channels
; iy: music state
banjo_init_opn:

    ; fm channels
    ld b, 3
    call banjo_init_opn_chs

    ; square channels
    call banjo_init_ay

    ret

; b: number of channels
; ix: channels
; iy: music state
banjo_init_opn_chs:

    ld l, 0
    ld de, _sizeof_channel

    music_init_opn_chs_loop:

        ; default volume
        ld (ix + channel.volume), 0x7f

        ; subchannel number
        ld (ix + channel.subchannel), l

        ; send key-offs
        ld h, 0x28
        call banjo_opn_write

        ; subchannel numbers should skip from 2 to 4
        ; e.g. 0, 1, 2, 4, 5, 6
        ld a, l
        cp a, 2
        jr nz, mi_opna_lt_ch3

            inc l
        
        mi_opna_lt_ch3:

        inc l

        ; move to next channel
        add ix, de

        djnz music_init_opn_chs_loop

    ret


; ix: channel
; iy: music state
banjo_init_sfx_channel_opn:

    ; default volume
    ld (ix + channel.volume), 0x7f

    ; subchannel number
    ld a, (iy + music_state.sfx_subchannel)
    ld (ix + channel.subchannel), a

    ret