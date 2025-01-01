
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
_banjo_mute_channel_opll: 

    ; silence channel
    ld a, channel.subchannel(ix)
    add a, #0x30
    out (#OPLL_REG_PORT), a
    ld a, #0xf
    out (#OPLL_DATA_PORT), a

	ret 

; ix: channel
; iy: music state
_banjo_unmute_channel_opll: 
        
    ; clear muted bit
    res CHAN_FLAG_BIT_MUTED, channel.flags(ix)

    ; get the current instrument number for this channel
    ld a, channel.instrument_num(ix)
    call music_instrument_change

    ret 

; mute all FM channels by sending key-offs
_banjo_mute_all_opll: 
	
    ; 9 opll channels
    ld b, #9

    ; first volume/patch register
    ld d, #0x30
    
    music_mute_opll_noteoff: 
    
        ; select volume register 
        ld a, d
        out (#OPLL_REG_PORT), a

        ; volume to zero
        ld a, #0xf
        out (#OPLL_DATA_PORT), a
        
        ; delay after opll data write
        push hl
        pop hl
        push hl
        pop hl
        push hl
        pop hl

        ; move to next channel's volume register
        inc d

        djnz music_mute_opll_noteoff

	ret 
