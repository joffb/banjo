
; banjo sound driver
; Joe Kennedy 2024

; ix: current channel
; iy: music state
music_volume_change_ay:

    ; for AY the quietest volume is 0 and the loudest is 15

    ; get channel volume into a and d
    ; also contains envelope flag in the upper nibble
    ld a, (ix + channel.volume)
    ld d, a

    ; check whether a volume macro is active and subtract the macro's value if it is
    bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
    jr z, mvc_ay_no_macro

        ; if subtracting the macro volume from the channel volume
        ; results in the carry flag, we need to set the volume to 0
        and a, 0x0f
        sub a, (ix + channel.volume_macro_vol)
        jr nc, mvc_ay_no_carry
        
            xor a, a

        mvc_ay_no_carry:
        ld c, a

    mvc_ay_no_macro:

    ; is the master volume less than 0x80?
    bit 7, (iy + music_state.master_volume)
    jr nz, mvcay_no_master_volume

        ; preserve current volume
        ld c, a
        
        ; make master volume a 4 bit number
        ld a, (iy + music_state.master_volume)
        rrca
        rrca
        rrca
        and a, 0xf

        ; invert
        neg
        add a, 0xf
        ld e, a

        ; sub from current volume total
        ld a, c
        sub a, e
        jr nc, mvcay_no_master_volume

            ; max out volume value at 0x0
            ld a, 0x00

    mvcay_no_master_volume:

    ; combine new volume with envelope flag
    ld c, a
    ld a, d
    and a, 0xf0
    or a, c

    ; preserve new volume in d
    ld d, a

    ; get AY_REG_WRITE in c
    ld c, (ix + channel.port)

    ; select volume register
    ld a, (ix + channel.subchannel)
    add a, 8
    out (c), a

    ; move c to AY_DATA_WRITE
    inc c

    ; get volume back into a and write to chip
    ld a, d
    out (c), a

	jp music_volume_change_ay_done