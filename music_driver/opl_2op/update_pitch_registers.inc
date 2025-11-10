
; banjo sound driver
; Joe Kennedy 2024

; b: current channel number
; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_opl_2op:

    push bc

    ; calculate note number
    call music_calc_fnum

    ; turn that into an fnum and octave
    ld hl, opl_2op_tone_lookup
    call music_fnum_lookup

    ; combine upper byte of fnum with octave in b register
    ld a, b
    rlca
    rlca

    ; set octave (block)
    and a, 0x01c
    or a, h

    ; set key on bit
    or a, 0x20
    ld h, a

    ; get f-number register to write to for this channel
    ld a, (ix + channel.subchannel)
    add a, 0xa0
    ld c, a
    out (OPL_REG_PORT), a
    
    ; lower 8 bits of f number
    ld a, l
    out (OPL_DATA_PORT), a
    
    call banjo_opl_write_delay
    
    ; get octave + key on register to write to for this channel
    ; by adding to base_reg from earlier
    ld a, c
    add a, 0x10
    out (OPL_REG_PORT), a
    
    ; write octave/9th bit of f number/note on bit
    ld a, h
    out (OPL_DATA_PORT), a

    ; keep it around in gp1 for use with note-off
    ld (ix + channel.gp1), h
    
    call banjo_opl_write_delay

    pop bc

    jp music_update_pitch_registers_opl_2op_done