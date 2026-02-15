
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_mute_channel_opll:

    ; silence channel
    ld a, (ix + channel.subchannel)
    add a, 0x30
    out (OPLL_REG_PORT), a
    ld a, 0xf
    out (OPLL_DATA_PORT), a

	ret

; ix: channel
; iy: music state
banjo_unmute_channel_opll:
        
    ; clear muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

    ; get the current instrument number for this channel
    ld a, (ix + channel.instrument_num)
    call music_instrument_change

    ret

; mute all FM channels by sending key-offs
banjo_mute_all_opll:
	
    ; 9 opll channels
    ld b, 9

    ; first volume/patch register
    ld d, 0x30
    
    music_mute_opll_noteoff:
    
        ; select volume register 
        ld a, d
        out (OPLL_REG_PORT), a

        ; volume to zero
        ld a, 0xf
        out (OPLL_DATA_PORT), a
        
        call banjo_opll_write_delay

        ; move to next channel's volume register
        inc d

        djnz music_mute_opll_noteoff

	ret