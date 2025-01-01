
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
_banjo_init_sn: 

    ; port number
    ld c, #SN76489_PORT

    _banjo_init_sn_preloop: 

    ; number of channels
    ld b, #4

    ; default sn noise to use ch3 tone and white noise
    ld a, #0xe7
    out (c), a

    ; ALF TEST
    ; additional write delay
    .ifdef BANJO_ALF
        call _banjo_sn_alf_wait
    .endif 

    music_init_sn_loop: 

        ; store the port number 
        ld channel.port(ix), c

        ; subchannel (preshifted << 5)
        ld a, #4
        sub a, b
        rrca 
        rrca 
        rrca 
        ld channel.subchannel(ix), a

        ; silence the chip channel
        or a, #0x9f
        out (c), a

        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call _banjo_sn_alf_wait
        .endif 

        ; set default channel volume
        ld channel.volume(ix), #0x00

        ; move to next channel
        ld de, #_sizeof_channel
        add ix, de

        djnz music_init_sn_loop

    ret 


; ix: channel
; iy: music state
_banjo_init_sn_2: 

    ; port number
    ld c, #SN76489_2_PORT

    jr _banjo_init_sn_preloop

; ix: channel
; iy: music state
_banjo_init_sn_3: 

    ; port number
    ld c, #SN76489_3_PORT

    jr _banjo_init_sn_preloop

; ix: channel
; iy: music state
_banjo_init_sn_4: 

    ; port number
    ld c, #SN76489_4_PORT

    jr _banjo_init_sn_preloop


; ix: channel
; iy: music state
_banjo_init_sfx_channel_sn: 

    ; store the port number 
    ld channel.port(ix), #SN76489_PORT

    ; subchannel (preshifted << 5)
    ld a, music_state.sfx_subchannel(iy)
    rrca 
    rrca 
    rrca 
    ld channel.subchannel(ix), a

    ; set default channel volume
    ld channel.volume(ix), #0x00

    ret 
