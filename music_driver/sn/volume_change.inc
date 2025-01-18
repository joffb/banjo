
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_sn:
    
    ; get channel volume
    ld a, (ix + channel.volume)

    .ifndef BANJO_MINIMAL

    ; check whether a volume macro is active and add the macro's value if it is
    bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
    jr z, mvcsn_no_macro

        ; isolate lower volume nibble and add macro volume
        add a, (ix + channel.volume_macro_vol)
        cp a, 0x10
        jr c, mvcsn_no_macro

            ; max out volume value at 0xf
            ld a, 0x0f
    
    mvcsn_no_macro:

    ; is the master volume less than 0x80?
    bit 7, (iy + music_state.master_volume)
    jr nz, mvcsn_no_master_volume

        ; preserve current volume
        ld c, a
        
        ; make master volume a 4 bit number
        ld a, (iy + music_state.master_volume)
        rrca
        rrca
        rrca
        and a, 0xf

        ; subtract from 15
        neg
        add a, 0xf

        ; add to current volume total
        add a, c
        cp a, 0x10
        jr c, mvcsn_no_master_volume

            ; max out volume value at 0xf
            ld a, 0x0f

    mvcsn_no_master_volume:
    
    .endif

    ; get channel (pre-shifted into correct position)
    or a, (ix + channel.subchannel)

    ; set latch and volume bit
    or a, 0x90

    ; load the port into c
    ld c, (ix + channel.port)
    
    ; update chip
    out (c), a

    ; ALF TEST
    ; additional write delay
    .ifdef BANJO_ALF
        call banjo_sn_alf_wait
    .endif

    jp music_volume_change_sn_done