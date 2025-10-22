
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_init_sn:

    ; check if this is a Game Gear and reset panning if it is
    ld a, (banjo_system_flags)
    bit SYS_FLAG_BIT_GG, a
    jr z, bisn_not_gg

        ; reset panning
        ld a, 0xff
        out (GAME_GEAR_PAN_PORT), a

    bisn_not_gg:

    ; port number
    ld c, SN76489_PORT

    banjo_init_sn_preloop:

    ; number of channels
    ld b, 4

    ; default sn noise to use ch3 tone and white noise
    ld a, 0xe7
    out (c), a

    ld a, 3
    ld (iy + music_state.noise_mode), a

    ; ALF TEST
    ; additional write delay
    .ifdef BANJO_ALF
        call banjo_sn_alf_wait
    .endif

    music_init_sn_loop:

        ; store the port number 
        ld (ix + channel.port), c

        ; subchannel (preshifted << 5)
        ld a, 4
        sub a, b
        rrca
        rrca
        rrca
        ld (ix + channel.subchannel), a

        ; silence the chip channel
        or a, 0x9f
        out (c), a

        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call banjo_sn_alf_wait
        .endif

        ; set default channel volume
        ld (ix + channel.volume), 0x0f

        ; move to next channel
        ld de, _sizeof_channel
        add ix, de

        djnz music_init_sn_loop

    ret


.ifdef BANJO_MULTIPLE_SN

    ; ix: channel
    ; iy: music state
    banjo_init_sn_2:

        ; port number
        ld c, SN76489_2_PORT

        jr banjo_init_sn_preloop

    ; ix: channel
    ; iy: music state
    banjo_init_sn_3:

        ; port number
        ld c, SN76489_3_PORT

        jr banjo_init_sn_preloop

    ; ix: channel
    ; iy: music state
    banjo_init_sn_4:

        ; port number
        ld c, SN76489_4_PORT

        jr banjo_init_sn_preloop

.endif

; ix: channel
; iy: music state
banjo_init_sfx_channel_sn:

    ; store the port number 
    ld (ix + channel.port), SN76489_PORT

    ; subchannel (preshifted << 5)
    ld a, (iy + music_state.sfx_subchannel)
    rrca
    rrca
    rrca
    ld (ix + channel.subchannel), a

    ; set default channel volume
    ld (ix + channel.volume), 0x0f

    ret