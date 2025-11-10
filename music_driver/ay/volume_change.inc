
; banjo sound driver
; Joe Kennedy 2024

; ix: current channel
; iy: music state
music_volume_change_ay:

    ; calculate volume w/ volume macros and master volume
    ; for AY the quietest volume is 0 and the loudest is 15
    ld a, (ix + channel.volume)
    ld c, 15
    call music_calculate_volume

    ; combine new volume with envelope flag
    ld c, a
    ld a, (ix + channel.volume)
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