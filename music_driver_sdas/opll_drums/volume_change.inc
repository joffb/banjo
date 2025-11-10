
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opll_drums: 

    ; get channel volume
    ld a, channel.volume(ix)
    ld c, #15
    call music_calculate_volume

    ; convert to opll volume
    neg 
    add a, #15

    ; preserve (possibly attenuated) volume
    ld d, a

    ; get current volume for the drum pair
    ; from the opll_drum_volumes table
    ld a, channel.subchannel(ix)
    sub a, #0x16
    
    add a, #<opll_drum_volumes
    ld l, a
    adc a, #>opll_drum_volumes
    sub a, l
    ld h, a

    ; this is the current volume for drum pair
    ld c, (hl)

    ; get volume mask and use it to check if the new volume will need to be shifted
    ld a, channel.gp3(ix)
    cp a, #0xf0
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
        ld a, channel.subchannel(ix)
        add a, #0x20
        out (#OPLL_REG_PORT), a

        ; write pair to chip
        ld a, c
        out (#OPLL_DATA_PORT), a

        jp music_volume_change_opll_drums_done
