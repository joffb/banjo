
; banjo sound driver
; Joe Kennedy 2023

; a: channel to mute
music_mute_channel:
	
    ; use a to get an offset into song_channel_ptrs into hl
	add a, a
    add a, <song_channel_ptrs
    ld l, a
    adc a, >song_channel_ptrs
    sub a, l
    ld h, a

    ; get actual pointer into channel in hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; mute channel by setting channel.muted = 1
    inc hl
    ld a, (hl)
    or a, CHAN_FLAG_MUTED
    ld (hl), a

	ret

; a: channel to mute
music_unmute_channel:
	
    ; use a to get an offset into song_channel_ptrs into hl
	add a, a
    add a, <song_channel_ptrs
    ld l, a
    adc a, >song_channel_ptrs
    sub a, l
    ld h, a

    ; get actual pointer into channel in hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; unmute channel by setting channel.muted = 0
    inc hl
    ld a, (hl)
    and a, ~CHAN_FLAG_MUTED
    ld (hl), a

	ret


; mute all FM channels by sending key offs
music_mute_all_fm:
	
	ld b, 9
	ld c, 0x20
	
	music_mute_fm_noteoff:
	
		ld a, c
		out (OPLL_REG_PORT), a
		ld a, 0
		out (OPLL_DATA_PORT), a
		
		inc c
		djnz music_mute_fm_noteoff

	ret

; mute all SN channels
music_mute_all_sn:

	; mute all psg channels
	ld a, 0x9f | (0 << 5)
	out (SN76489_PORT), a
    out (SN76489_2_PORT), a

	ld a, 0x9f | (1 << 5)
	out (SN76489_PORT), a
    out (SN76489_2_PORT), a

	ld a, 0x9f | (2 << 5)
	out (SN76489_PORT), a
    out (SN76489_2_PORT), a

	ld a, 0x9f | (3 << 5)
	out (SN76489_PORT), a
    out (SN76489_2_PORT), a

	ret