
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_mute_channel_opl_2op:

    ; silence channel via key-offs
    ld a, (ix + channel.subchannel)
    add a, 0xb0
    out (OPL_REG_PORT), a
    xor a, a
    out (OPL_DATA_PORT), a

	ret

; ix: channel
; iy: music state
banjo_unmute_channel_opl_2op:
        
    ; clear muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

    ; get the current instrument number for this channel
    ld a, (ix + channel.instrument_num)
    call music_instrument_change

    ret

; mute all FM channels by sending key-offs
banjo_mute_all_opl_2op:
	
    ; 9 opll channels
    ld b, 9

    ; first key-on/off register
    ld d, 0x80
    
    music_mute_opl_2op_noteoff:
    
        ; select key-on/off register 
        ld a, d
        out (OPL_REG_PORT), a

        ; note off
        xor a, a
        out (OPL_DATA_PORT), a
        
        call banjo_opl_write_delay

        ; move to next channel's volume register
        inc d

        djnz music_mute_opl_2op_noteoff

	ret