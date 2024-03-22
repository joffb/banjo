
; banjo sound driver
; Joe Kennedy 2023

; a: channel to mute
_banjo_mute_song_channel: 
	
    ; use a to get an offset into song_channel_ptrs into hl
	add a, a
    add a, #<song_channel_ptrs
    ld l, a
    adc a, #>song_channel_ptrs
    sub a, l
    ld h, a

    ; get actual pointer into channel in hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; this falls through!

; hl: pointer to channel to mute
music_mute_channel: 

    ; mute channel by setting channel.muted = 1
    set CHAN_FLAG_BIT_MUTED, (hl)

    ; get chip type
    inc hl
    ld a, (hl)

    ; SN 
    cp a, #CHAN_SN76489
    jr nz, mmc_check_opll

        ; get subchannel
        inc hl
        ld a, (hl)
        or a, #0x9f

        ; get port 
        inc hl
        ld c, (hl)

        ; write to chip
        out (c), a
        jr mmc_done

    ; FM - mute channel by sending key-off
    mmc_check_opll: 
    cp a, #CHAN_OPLL
    jr nz, mmc_done

        ; get subchannel
        inc hl
        ld a, (hl)

        add a, #0x20
		out (#OPLL_REG_PORT), a
		ld a, #0
		out (#OPLL_DATA_PORT), a

        jr mmc_done

    mmc_done: 
	ret 

; a: channel to mute
_banjo_unmute_song_channel: 
	
    ; use a to get an offset into song_channel_ptrs into hl
	add a, a
    add a, #<song_channel_ptrs
    ld l, a
    adc a, #>song_channel_ptrs
    sub a, l
    ld h, a

    ; get actual pointer into channel in hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; unmute channel by setting channel.muted = 0
    res CHAN_FLAG_BIT_MUTED, (hl)

    ; get the type of channel
    inc hl
    ld a, (hl)

    ; if it's an opll channel we want to restore to instrument
    cp a, #CHAN_OPLL
    jr nz, music_unmute_channel_done

        ; preserve ix and iy
        push ix
        push iy

        ; get channel in ix
        dec hl
        push hl
        pop ix

        ; get song state in iy
        ld iy, #_song_state

        .ifndef QUEUES_OFF
        ; change to song bank
        ld a, music_state.bank(iy)
        ld (SLOT_2_BANK_CHANGE), a
        .endif 

        ; get the current instrument number for this channel
        ld a, channel.instrument_num(ix)
        call music_instrument_change

        ; restore ix and iy
        pop iy
        pop ix

    music_unmute_channel_done: 
	ret 


; mute all FM channels by sending key-offs
music_mute_all_fm: 
	
	ld b, #9
	ld d, #0x20
	
	music_mute_fm_noteoff: 
	
		ld a, d
		out (#OPLL_REG_PORT), a
		ld a, #0
		out (#OPLL_DATA_PORT), a
		
        ; delay after opll data write
        push hl
        pop hl
        push hl
        pop hl
        push hl
        pop hl

		inc d
		djnz music_mute_fm_noteoff

	ret 

; mute all SN channels
; writes to both the top SN port for the SMS
; and the port for the second SN in the System E
; on SMS this second SN port just goes to the SN again
music_mute_all_sn: 

	; mute all psg channels
	ld a, #0x9f|(0<<5)
	out (#SN76489_PORT), a
    out (#SN76489_2_PORT), a

	ld a, #0x9f|(1<<5)
	out (#SN76489_PORT), a
    out (#SN76489_2_PORT), a

	ld a, #0x9f|(2<<5)
	out (#SN76489_PORT), a
    out (#SN76489_2_PORT), a

	ld a, #0x9f|(3<<5)
	out (#SN76489_PORT), a
    out (#SN76489_2_PORT), a

	ret 
