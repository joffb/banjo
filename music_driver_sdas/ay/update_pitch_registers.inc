
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music_state
music_update_pitch_registers_ay: 

    push bc

    ; calculate note number
    call music_calc_fnum

    ; turn that into an fnum and octave
    ld hl, #ay_tone_lookup
    call music_fnum_lookup

    ; shift fnum to the correct octave
    call music_fnum_shift_octave

    ; get fine pitch fnum register
    ; keep it in c
    ld a, channel.subchannel(ix)
    add a, a
    ld c, a
    out (#AY_REG_WRITE), a

    ; write fine pitch
    ld a, l
    out (#AY_DATA_WRITE), a
    
    ; get coarse pitch register
    ld a, c
    inc a
    out (#AY_REG_WRITE), a

    ; write coarse pitch
    ld a, h
    out (#AY_DATA_WRITE), a

    pop bc

    jp music_update_pitch_registers_ay_done
