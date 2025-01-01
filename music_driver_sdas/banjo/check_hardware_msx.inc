
; banjo sound driver
; Joe Kennedy 2023

; b: slot selector
; c: character count
; hl: slot address
; de: string address
bch_string_search: 

    ; get value at hl in slot selected by b
    push bc
    push de

    ; slot # is b - 1
    ; also set bit 7 for expanded slots
    ld a, b
    dec a
    or a, #0x80
    call BANJO_RDSLT

    pop de
    pop bc

    ; check if value we got == next magic string value
    ex de, hl
    cp a, (hl)
    jr z, bch_string_search_match

        ; failed
        ld a, #0
        ret 

    bch_string_search_match: 

    ; restore original hl and de and
    ; move them to the next char
    ex de, hl
    inc hl
    inc de

    ; continue if all not yet matched (c > 0)
    dec c
    jr nz, bch_string_search

    ; otherwise, matched
    ld a, #1
    ret 

; check for presence of hardware and set bits in the banjo_has_chips
; variable, corresponding to what was found
_banjo_check_hardware: 
    
    ; ay3 is present by default for msx
    ld a, #BANJO_HAS_AY
    ld (_banjo_has_chips), a

    ; clear system flags
    ld a, #0
    ld (_banjo_system_flags), a

    ; opll detection

    ; search through all slots at 0x4018 for the "APRLOPLL" string
    ld b, #0x10

    bch_msx_aprlopll_search: 

        ld hl, #MSX_FMPAC_STRING_LOC
        ld de, #bch_msx_opll_magic_string
        ld c, #8
        call bch_string_search

        ; check whether string search matched
        or a, a
        jr z, bch_msx_aprlopll_not_matched

            ; matched
            ld a, (_banjo_has_chips)
            or a, #BANJO_HAS_OPLL
            ld (_banjo_has_chips), a

            jr bch_msx_opll_done

        bch_msx_aprlopll_not_matched: 

            ; loop to next slot
            djnz bch_msx_aprlopll_search


    ; search through all slots at 0x401C for the "OPLL" string
    ld b, #0x10

    bch_msx_opll_search: 

        ld hl, #MSX_FMPAC_STRING_LOC+4
        ld de, #bch_msx_opll_magic_string+4
        ld c, #4
        call bch_string_search

        ; check whether string search is completed
        or a, a
        jr z, bch_msx_opll_not_matched

            ld a, (_banjo_has_chips)
            or a, #BANJO_HAS_OPLL
            ld (_banjo_has_chips), a

            ; get current value of byte at 0x7ff6
            ; in the fmpac slot we've found
            push bc
            ld a, b
            dec a
            or a, #0x80
            ld hl, #MSX_FMPAC_IO_ENABLE
            call BANJO_RDSLT
            pop bc

            ; enable i/o ports on fm-pac
            ; by setting bit 0 of 0x7ff6
            ; in the fmpac slot we've found
            or a, #1
            ld e, a

            ld a, b
            dec a
            or a, #0x80
            ld hl, #MSX_FMPAC_IO_ENABLE
            call BANJO_WRTSLT

            jr bch_msx_opll_done

        bch_msx_opll_not_matched: 

            ; loop to next slot
            djnz bch_msx_opll_search

    bch_msx_opll_done: 

    ret 
