
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_sn:

    ; calculate volume w/ volume macros and master volume
    ld a, (iy + music_state.master_volume)
    ld c, 0xf
    call music_calculate_volume_0x0f

    ; convert to sn volume 
    neg
    add a, c

    ; get channel (pre-shifted into correct position)
    or a, (ix + channel.subchannel)

    ; set latch and volume bit
    or a, 0x90

    ; update chip
    .ifdef BANJO_SMS

        out (SN76489_PORT), a

    .else

        ld c, (ix + channel.port)
        out (c), a

    .endif

    ; ALF TEST
    ; additional write delay
    .ifdef BANJO_ALF
        call banjo_sn_alf_wait
    .endif

    jp music_volume_change_sn_done