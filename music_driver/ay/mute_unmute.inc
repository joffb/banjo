
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music_state
banjo_mute_channel_ay:

    ; get AY_REG_WRITE in c
    ld c, (ix + channel.port)

    ; silence channel
    ld a, (ix + channel.subchannel)
    add a, 8
    out (c), a

    ; move c to AY_DATA_WRITE
    inc c

    ; write 0 to volume/env register 
    ld a, 0
    out (c), a
    
    ret

; ix: channel
; iy: music state
banjo_unmute_channel_ay:

    ; reset muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    
    ret

; mute all ay channels
banjo_mute_all_ay:

    ; first volume register
    ld d, 8
    ld b, 3

    music_mute_all_ay_loop:
        
        ; select volume register
        ld a, d
        out (AY_REG_WRITE), a

        ; silence channel
        ld a, 0
        out (AY_DATA_WRITE), a

        ; move to next volume register
        inc d

        djnz music_mute_all_ay_loop

    ret