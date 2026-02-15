
; banjo sound driver
; Joe Kennedy 2023

; check for Sega System E by seeing if there's 16kb of RAM 
; rather than 8kb with 0xe000 being a mirror of 0xc000
banjo_check_hardware:

    ; get byte at 0xc000 and save it
    ld de, 0xe000
    ld hl, 0xc000
    ld a, (hl)
    push af

    ; number of values to write for test
    ld b, 8

    ; number of matched mirror read/writes
    ld c, 0

    bch_check_system_e:

        ; write b to 0xc000
        ld (hl), b

        ; read 0xe000 and compare it
        ld a, (de)
        cp a, (hl)
        jr z, bch_not_mirrored

            ; 0xc000 write != 0xe000 read
            inc c

        bch_not_mirrored:
        djnz bch_check_system_e

    ; restore byte at 0xc000
    pop af
    ld (hl), a

    ; c == 0 if all 0xe000 reads matched the 0xc000 writes
    ld a, c
    or a, a
    jr z, bch_check_system_e_done

        ld a, (banjo_has_chips)
        or a, BANJO_HAS_DUAL_SN
        ld (banjo_has_chips), a

        ld a, (banjo_system_flags)
        or a, SYS_FLAG_SYSTEM_E
        ld (banjo_system_flags), a

    bch_check_system_e_done:

    ret