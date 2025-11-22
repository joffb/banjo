
; banjo sound driver
; Joe Kennedy 2025

; ix: channel
; iy: music state
banjo_init_opna:

    ld a, OPN_REG_PORT
    ld (banjo_opn_port), a
    ld a, OPNA_REG_PORT
    ld (banjo_opna_port), a

    ; enable six voices of opna fm
    ; also enables interrupts
    ld h, 0x29
    ld l, 0x80 | 0x1f
    call banjo_opn_write

    ; rhythm master volume
    ld h, 0x11
    ld l, 0x3f
    call banjo_opn_write

    ; default lfo val
    ld h, 0x22
    ld l, 0x08
    call banjo_opn_write

    ; fm channels
    ld b, 6
    call banjo_init_opn_chs

    ; square channels
    call banjo_init_ay

    ; rhythm channels
    call banjo_init_opna_rhythm

    ret

