
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio: 

    ; flag that we need to update the pitch
    set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

    ; move arpeggio pos along
    ; check whether arp_pos == 3
    ld a, channel.arpeggio_pos(ix)
    inc a
    cp a, #3
    jr nz, mpct_inc_arp_pos

        ; if it's 3, reset arpeggio position to 0
        ld channel.arpeggio_pos(ix), #0
        
        ret 

    ; it's < 3
    mpct_inc_arp_pos: 

        ; update arp position
        ld channel.arpeggio_pos(ix), a

        ; rotate the nibbles of the arpeggio to alternate
        ; the note offset between tics
        ld a, channel.arpeggio(ix)
        rrca 
        rrca 
        rrca 
        rrca 
        ld channel.arpeggio(ix), a

        ret 
