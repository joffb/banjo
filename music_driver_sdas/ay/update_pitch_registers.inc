
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

    ; get port number in c
    ld c, channel.port(ix)

    ; get fine pitch fnum register
    ; keep it in e
    ld a, channel.subchannel(ix)
    add a, a
    ld e, a
    out (c), e

    ; write fine pitch
    inc c
    out (c), l
    dec c
    
    ; get coarse pitch register
    inc e
    out (c), e

    ; write coarse pitch
    inc c
    out (c), h

    pop bc

    jp music_update_pitch_registers_ay_done
