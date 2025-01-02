
; banjo sound driver
; Joe Kennedy 2024

; b: current channel number
; ix: channel
; iy: music_state
music_update_pitch_registers_ay: 

    ; get note fnum in de
    ld hl, #ay_tone_lookup
    call music_calc_fnum

    ; get fine pitch fnum register
    ; keep it in c
    ld a, channel.subchannel(ix)
    add a, a
    ld c, a
    out (#AY_REG_WRITE), a

    ; write fine pitch
    ld a, e
    out (#AY_DATA_WRITE), a
    
    ; get coarse pitch register
    ld a, c
    inc a
    out (#AY_REG_WRITE), a

    ; write coarse pitch
    ld a, d
    out (#AY_DATA_WRITE), a

    jp music_update_pitch_registers_ay_done
