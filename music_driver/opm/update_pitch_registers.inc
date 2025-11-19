
; banjo sound driver
; Joe Kennedy 2024

; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_opm:

    push bc

    ; keycode register in h
    ld a, (ix + channel.subchannel)
    add a, 0x28
    ld h, a

    ; get keycode
    ; note number (0-11) in lower nibble and octave in upper nibble
    ; using divmod table
    ld a, (ix + channel.freq + 1)
    and a, 0x7f
    add a, <music_divmod12_opm
    ld e, a
    adc a, >music_divmod12_opm
    sub a, e
    ld d, a

    ; rotate nibbles
    ld a, (de)
    
    ; write to register
    ld l, a
    call banjo_opm_write

    ; key fraction register in h
    ld a, (ix + channel.subchannel)
    add a, 0x30
    ld h, a

    ; get key fraction in l
    ld l, (ix + channel.freq)

    ; write to register
    call banjo_opm_write

    pop bc

    jp music_update_pitch_registers_opm_done
    