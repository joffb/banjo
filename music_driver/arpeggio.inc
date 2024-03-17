
; banjo sound driver
; Joe Kennedy 2024

music_update_arpeggio:

    ; flag that we need to update the pitch
    set CHAN_FLAG_BIT_PITCH_CHANGED, (ix + channel.flags)

    ; move arpeggio pos along
    ; check whether arp_pos == 3
    ld a, (ix + channel.arpeggio_pos)
    inc a
    cp a, 3
    jr nz, mpct_inc_arp_pos

        ; if it's 3, reset arpeggio position to 0
        ld (ix + channel.arpeggio_pos), 0
        
        ret

    ; it's < 3
    mpct_inc_arp_pos:

        ; update arp position
        ld (ix + channel.arpeggio_pos), a

        ; rotate the nibbles of the arpeggio to alternate
        ; the note offset between tics
        ld a, (ix + channel.arpeggio)
        rrca
        rrca
        rrca
        rrca
        ld (ix + channel.arpeggio), a

        ret