

_banjo_opn_wait: 
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl
	ret 

; c : port number to check
music_detect_opna: 

    ; this check is inspired by what "Wanderers from Super Scheme" does

    ld b, #0

    ; search for opna in 0x44 ports (internal)
    ld c, #OPN_REG_PORT_1
    ld a, #0xff
    out (c), a

    push hl
    pop hl

    ; move to data port and read
    inc c
    in a, (c)

    ; if we read 1 then we have found an OPNA
    dec a
    jr nz, mdo_opna_not_port_1

        ld a, #OPN_REG_PORT_1
        ld (_banjo_opn_port), a
        ld a, #OPNA_REG_PORT_1
        ld (_banjo_opna_port), a

        ld a, #BANJO_HAS_OPN|BANJO_HAS_OPNA
        ld (_banjo_has_chips), a

        ret 

    mdo_opna_not_port_1: 

    ; search for opna in 0xa8 ports (soundboard)
    ld c, #OPN_REG_PORT_2
    ld a, #0xff
    out (c), a

    push hl
    pop hl

    ; move to data port and read
    inc c
    in a, (c)

    ; if we read 1 then we have found an OPNA
    dec a
    jr nz, mdo_opna_not_port_2

        ld a, #OPN_REG_PORT_2
        ld (_banjo_opn_port), a
        ld a, #OPNA_REG_PORT_2
        ld (_banjo_opna_port), a

        ld a, #BANJO_HAS_OPN|BANJO_HAS_OPNA
        ld (_banjo_has_chips), a

        ret 

    mdo_opna_not_port_2: 

        ; if we don't have an opna in either port
        ; assume we have an internal OPN
        ld a, #OPN_REG_PORT_1
        ld (_banjo_opn_port), a

        ld a, #BANJO_HAS_OPN
        ld (_banjo_has_chips), a

        ret 

; check for presence of hardware and set bits in the banjo_has_chips
; variable, corresponding to what was found
_banjo_check_hardware: 

    ; disable and reset timers
    ld a, #0x27
    out (c), a
    inc c
    ld a, #0x10|0x20
    out (c), a
    dec c
    
    call _banjo_opn_wait

    call music_detect_opna

    ret 
