
; banjo sound driver
; Joe Kennedy 2024

; ix: current channel
; iy: music state
music_volume_change_ay:

    ; calculate volume w/ volume macros and master volume
    ld a, (iy + music_state.master_volume)
    call music_calculate_volume_0x0f

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

    ; write volume to chip
    inc c
    out (c), d

	jp music_volume_change_ay_done