
; banjo sound driver
; Joe Kennedy 2024

; b: current channel number
; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_opll:

    ; get note fnum in de
    ld hl, fm_tone_lookup
    call music_calc_fnum

    ; get f-number register to write to for this channel
    ld a, (ix + channel.subchannel)
    add a, 0x10
    ld c, a
    out (OPLL_REG_PORT), a
    
    ; lower 8 bits of f number
    ld a, e
    out (OPLL_DATA_PORT), a
    
    call banjo_opll_write_delay
    
    ; get octave + key on register to write to for this channel
    ; by adding to base_reg from earlier
    ld a, c
    add a, 0x10
    out (OPLL_REG_PORT), a
    
    ; get octave and 9th bit of f number
    ld a, d
    
    ; set key on bit and write
    and a, 0xf
    or a, 0x10
    out (OPLL_DATA_PORT), a
    
    call banjo_opll_write_delay

    jp music_update_pitch_registers_opll_done