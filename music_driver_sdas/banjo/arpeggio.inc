
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio: 

    ; decrement arpeggio position and check if it has hit 0
    dec channel.effect1(ix)
    jr z, mpct_arp_zero

        ; arpeggio_pos == 2? if not we don't shift
        bit 1, channel.effect1(ix)
        jr z, mpct_arp_no_shift

            ld a, channel.effect2(ix)
            ret 

        mpct_arp_no_shift: 

            ld a, channel.effect3(ix)
            ret 

    mpct_arp_zero: 

        ; if so, reset arpeggio position to 3
        ld channel.effect1(ix), #3

        ; return 0
        xor a, a

        ret 
        
