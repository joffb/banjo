
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
_banjo_init_ay: 

    ld b, #3
    ld c, #AY_REG_WRITE

    music_init_ay_loop: 

        ; get subchannel number
        ld a, #3
        sub a, b
        ld channel.subchannel(ix), a

        ; store port
        ld channel.port(ix), c

        ; silence the chip channel
        add a, #8
        out (c), a

        ; move c to AY_DATA_WRITE
        inc c
        ld a, #0
        out (c), a

        ; move c back to AY_REG_WRITE
        dec c

        ; set default channel volume
        ld channel.volume(ix), #0x0f

        ; move to next channel
        ld de, #_sizeof_channel
        add ix, de

        djnz music_init_ay_loop

    ; return to music_play function
    ret 


; ix: channel
; iy: music state
_banjo_init_sfx_channel_ay: 

    ; subchannel
    ld a, music_state.sfx_subchannel(iy)
    ld channel.subchannel(ix), a

    ; set default channel volume
    ld channel.volume(ix), #0x0f

    ; return to music_play function
    ret 
