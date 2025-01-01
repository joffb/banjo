
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_mute_channel_opll_drums:

	ret

; ix: channel
; iy: music state
banjo_unmute_channel_opll_drums:

    ; clear muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

    ret


; mute all FM channels by sending key-offs
banjo_mute_all_opll_drums:
	
    ; 9 opll channels
    ld b, 9

    ; first volume/patch register
    ld d, 0x30
    
    music_mute_opll_drums_noteoff:
    
        ; select volume register 
        ld a, d
        out (OPLL_REG_PORT), a

        ; volume to zero
        ld a, 0xf
        out (OPLL_DATA_PORT), a
        
        ; delay after opll data write
        push hl
        pop hl
        push hl
        pop hl
        push hl
        pop hl

        ; move to next channel's volume register
        inc d

        djnz music_mute_opll_drums_noteoff

	ret