
; banjo sound driver
; Joe Kennedy 2024

; b: current channel number
; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_opn:

    push bc

    ; calculate note number
    call music_calc_fnum

    ; turn that into an fnum and octave
    ld hl, opn_tone_lookup
    call music_fnum_lookup

    ; put fnum and octave into de
    ex de, hl

    ; combine upper byte of fnum with octave in b register
    ld a, b
    rlca
    rlca
    rlca

    ; set octave (block)
    and a, 0x38
    or a, d

    ; write fnum + block register
    ld h, 0xa4
    ld l, a
    call banjo_opn_write_auto

    ; write lower 8 bits of f number
    ld h, 0xa0
    ld l, e
    call banjo_opn_write_auto

    pop bc

    jp music_update_pitch_registers_opn_done