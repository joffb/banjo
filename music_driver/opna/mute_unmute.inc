
; banjo sound driver
; Joe Kennedy 2025

; mute all FM channels by sending key-offs
banjo_mute_all_opna:
	
    ; 6 opn channels
    ld b, 6
    ld l, 0

    music_mute_all_opna_noteoff:
    
        ld h, 0x28
        call banjo_opn_write
        
        ; subchannel numbers should skip from 2 to 4
        ; e.g. 0, 1, 2, 4, 5, 6
        ld a, l
        cp a, 2
        jr z, mma_opna_lt_ch3

            inc l
        
        mma_opna_lt_ch3:

        ; next channel
        inc l

        djnz music_mute_all_opna_noteoff

	ret