
; automatically select opn or opna port
; depending on subchannel bit 2
; offset register number by subchannel & 3
; h: register
; l: value
banjo_opn_write_auto:

    push bc
    
    ld c, (ix + channel.subchannel)

    ; offset register number
    ld a, c
    and a, 0x3
    add a, h
    ld h, a
    
    ; opn or opna write?
    bit 2, c
    jr nz, banjo_opn_wa_opna

        ld a, (banjo_opn_port)
        ld c, a
        jr banjo_opn_do_write

    banjo_opn_wa_opna:

        ld a, (banjo_opna_port)
        ld c, a
        jr banjo_opn_do_write

; h: register
; l: value
banjo_opna_write:
    
    push bc

    ld a, (banjo_opna_port)
    ld c, a
    
    jr banjo_opn_do_write

; h: register
; l: value
banjo_opn_write:

    push bc

    ld a, (banjo_opn_port)
    ld c, a

banjo_opn_do_write:

    ; wait for not busy
    bopn_write_reg_busy_loop:
        in a, (c)
        jp m, bopn_write_reg_busy_loop
    
    ; select register
    out (c), h
    inc c
    nop

    ; write value
    out (c), l

    pop bc

    ret

; delay after writing to opll chip
banjo_opn_write_delay_long:
    push hl
    pop hl
    push hl
    pop hl
banjo_opn_write_delay_short:
    push hl
    pop hl
    push hl
    pop hl
    ret