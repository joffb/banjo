
; banjo sound driver
; Joe Kennedy 2024

; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_opll: 

    push bc

    ; calculate note number
    call music_calc_fnum

    ; turn that into an fnum and octave
    ld hl, #fm_tone_lookup
    call music_fnum_lookup

    ; combine upper byte of fnum with octave in b register
    ld a, b
    rlca 

    ; set octave (block)
    and a, #0x0e
    or a, h

    ; set key on bit
    or a, #0x10
    ld h, a

    ; get f-number register to write to for this channel
    ld a, channel.subchannel(ix)
    add a, #0x10
    ld c, a
    out (#OPLL_REG_PORT), a
    
    ; lower 8 bits of f number
    ld a, l
    out (#OPLL_DATA_PORT), a
    
    call _banjo_opll_write_delay
    
    ; get octave + key on register to write to for this channel
    ; by adding to base_reg from earlier
    ld a, c
    add a, #0x10
    out (#OPLL_REG_PORT), a
    
    ; write octave/9th bit of f number/note on bit
    ld a, h
    out (#OPLL_DATA_PORT), a
    
    call _banjo_opll_write_delay

    pop bc

    jp music_update_pitch_registers_opll_done
