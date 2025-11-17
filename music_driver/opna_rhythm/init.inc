
; banjo sound driver
; Joe Kennedy 2025

banjo_init_opna_rhythm:
    
    ; note-on/off
    ld h, 1
    ; subchannel
    ld l, 0

    ld de, _sizeof_channel

    ld b, 6

    mi_opna_rhythm_loop:

        ; default volume
        ld (ix + channel.volume), 0x1f

        ; subchannel number
        ld (ix + channel.subchannel), l
        inc l

        ; note-on/off bit
        ld (ix + channel.gp1), h
        sla h

        ; move to next channel
        add ix, de

        djnz mi_opna_rhythm_loop

    ; set volumes
    ld h, 0x18
    ld l, 0x1f | 0x80 | 0x40

    ld b, 6

    mi_opna_r_vol_loop:

        call banjo_opn_write
        inc h
        
        djnz mi_opna_r_vol_loop

    ret