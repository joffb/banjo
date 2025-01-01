
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio:

    ; decrement arpeggio position and check if it has hit 0
    dec (ix + channel.arpeggio_pos)
    jr z, mpct_arp_zero

        ; rotate the nibbles of the arpeggio to alternate
        ; the note offset between tics
        ld a, (ix + channel.arpeggio)

        ; arpeggio_pos == 2? if not we don't shift
        bit 1, (ix + channel.arpeggio_pos)
        jr z, mpct_arp_no_shift

            rrca
            rrca
            rrca
            rrca

        mpct_arp_no_shift:

            ; return lower nibble
            and a, 0xf

            ret

    mpct_arp_zero:

        ; if so, reset arpeggio position to 3
        ld (ix + channel.arpeggio_pos), 3

        ; return 0
        ld a, 0

        ret
