
; banjo sound driver
; Joe Kennedy 2024

; ix: current channel
; iy: music state
music_volume_change_ay: 

    ; get channel volume into a and d
    ; also contains envelope flag in the upper nibble
    ld a, channel.volume(ix)
    ld d, a

    ; check whether a volume macro is active and subtract the macro's value if it is
    bit CHAN_FLAG_BIT_VOLUME_MACRO, channel.flags(ix)
    jr z, mvc_ay_no_macro

        ; if subtracting the macro volume from the channel volume
        ; results in the carry flag, we need to set the volume to 0
        and a, #0x0f
        sub a, channel.volume_macro_vol(ix)
        jr nc, mvc_ay_no_carry
        
            xor a, a

        mvc_ay_no_carry: 
        ld c, a

        ; combine new volume with envelope flag
        ld a, d
        and a, #0xf0
        or a, c

    mvc_ay_no_macro: 

    ; preserve new volume in d
    ld d, a

    ; get AY_REG_WRITE in c
    ld c, channel.port(ix)

    ; select volume register
    ld a, channel.subchannel(ix)
    add a, #8
    out (c), a

    ; move c to AY_DATA_WRITE
    inc c

    ; get volume back into a and write to chip
    ld a, d
    out (c), a

	jp music_volume_change_ay_done
