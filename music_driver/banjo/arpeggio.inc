
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio:

    ; decrement arpeggio position and check if it has hit 0
    dec (ix + channel.effect1)
    jr z, mpct_arp_zero

        ; arpeggio_pos == 2? if not we don't shift
        bit 1, (ix + channel.effect1)
        jr z, mpct_arp_no_shift

            ld a, (ix + channel.effect2)
            ret

        mpct_arp_no_shift:

            ld a, (ix + channel.effect3)
            ret

    mpct_arp_zero:

        ; if so, reset arpeggio position to 3
        ld (ix + channel.effect1), 3

        ; return 0
        xor a, a

        ret
        