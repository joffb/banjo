
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opll_drums:

    ; get channel volume
    ld a, (ix + channel.volume)

    ; is the master volume less than 0x80?
    bit 7, (iy + music_state.master_volume)
    jr nz, mvcopll_drums_no_master_volume

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
        jr c, mvcopll_drums_no_master_volume

            ; max out volume value at 0xf
            ld a, 0x0f

    mvcopll_drums_no_master_volume:

    ; preserve (possibly attenuated) volume
    ld d, a

    ; get current volume for the drum pair
    ; from the opll_drum_volumes table
    ld a, (opll_drum_pitch_register)
    sub a, 0x16
    
    add a, <opll_drum_volumes
    ld l, a
    adc a, >opll_drum_volumes
    sub a, l
    ld h, a

    ; this is the current volume for drum pair
    ld c, (hl)

    ; get volume mask and use it to check if the new volume will need to be shifted
    ld a, (opll_drum_volume_mask)
    cp a, 0xf0
    jr z, mvc_opll_no_shift
        
        ; mask off volume pair and preserve
        and a, c
        ld c, a

        ; shift volume nto upper nibble and combine with volume pair
        ld a, d
        rrca
        rrca
        rrca
        rrca
        or a, c

        jr mvc_opll_shift_done

    mvc_opll_no_shift:

        ; mask off volume pair and combine with volume
        and a, c
        or a, d

    mvc_opll_shift_done:

        ; store new volume pair and preserve it for a sec
        ld (hl), a
        ld c, a

        ; get rhythm volume port to write to
        ld a, (opll_drum_pitch_register)
        add a, 0x20
        out (OPLL_REG_PORT), a

        ; write pair to chip
        ld a, c
        out (OPLL_DATA_PORT), a

        jp music_volume_change_opll_drums_done