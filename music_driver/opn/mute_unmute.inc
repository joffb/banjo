
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_mute_channel_opn:

    ; silence channel via key-offs
    ld h, 0x28
    ld l, (ix + channel.subchannel)
    call banjo_opn_write

	ret

; ix: channel
; iy: music state
banjo_unmute_channel_opn:
        
    ; clear muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

    ; get the current instrument number for this channel
    ld a, (ix + channel.instrument_num)
    call music_instrument_change

    ret

; mute all FM channels by sending key-offs
banjo_mute_all_opn:
	
    ; 3 opn channels
    ld b, 3
    ld l, 0

    music_mute_all_opn_noteoff:
    
        ld h, 0x28
        call banjo_opn_write

        ; next channel
        inc l

        djnz music_mute_all_opn_noteoff

	ret