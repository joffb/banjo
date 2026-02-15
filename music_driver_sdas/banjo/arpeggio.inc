
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio: 

    ; decrement arpeggio position and check if it has gone negative
    dec channel.effect1(ix)
    jp m, mpct_arp_reset

        ; if it wasn't zero after decrementing, use the first value
        jr z, mpct_arp_val2

            ld a, channel.effect2(ix)
            ret 

        ; if it was zero after decrementing, use the second value
        mpct_arp_val2: 

            ld a, channel.effect3(ix)
            ret 

    mpct_arp_reset: 

        ; arp position has gone negative, reset it to 2
        ld channel.effect1(ix), #2

        ; return 0
        xor a, a

        ret 
        
