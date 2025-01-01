
; banjo sound driver
; Joe Kennedy 2023

; ix: channel
; iy: state
; a: new instrument number
music_instrument_change:

	; store the new instrument number in the channel
	ld (ix + channel.instrument_num), a
	
	; get pointer to instrument address table in hl
	ld l, (iy + music_state.instrument_ptrs)
	ld h, (iy + music_state.instrument_ptrs + 1)
	
	; offset to get instrument pointer by adding instrument_num * 2 to hl
    add a, a 
	add a, l
	ld l, a
	adc a, h
	sub a, l
	ld h, a

    ; get pointer to instrument into hl
	ld a, (hl)
	inc hl
	ld h, (hl)	
	ld l, a
	
	; use c as a flags variable
	ld a, (ix + channel.flags)
	and a, ~(CHAN_FLAG_VOLUME_MACRO | CHAN_FLAG_EX_MACRO)
	ld c, a

	; volume macro
	; copy over macro info

	ld a, (hl)
	ld (ix + channel.volume_macro_len), a

	; check if volume_macro_len > 0
	or a, a
	jr z, mic_no_volume_macro

		inc hl
		ld a, (hl)
		ld (ix + channel.volume_macro_loop), a

		inc hl
		ld a, (hl)
		ld (ix + channel.volume_macro_ptr), a

		inc hl
		ld a, (hl)
		ld (ix + channel.volume_macro_ptr + 1), a

		; set initial macro position
		ld (ix + channel.volume_macro_pos), 0

		; set volume macro flag
		set CHAN_FLAG_BIT_VOLUME_MACRO, c

		jr mic_volume_macro_done

	mic_no_volume_macro:

		inc hl
		inc hl
		inc hl

	mic_volume_macro_done:

	; ex macro
	; copy over macro info

	; always copy over ex_macro_type
	inc hl
	ld a, (hl)
	ld (ix + channel.ex_macro_type), a

	inc hl
	ld a, (hl)
	ld (ix + channel.ex_macro_len), a

	; check if macro_len > 0
	or a, a
	jr z, mic_no_ex_macro

		inc hl
		ld a, (hl)
		ld (ix + channel.ex_macro_loop), a

		inc hl
		ld a, (hl)
		ld (ix + channel.ex_macro_ptr), a

		inc hl
		ld a, (hl)
		ld (ix + channel.ex_macro_ptr + 1), a

		; set initial macro position
		ld (ix + channel.ex_macro_pos), 0

		; set ex macro flag
		set CHAN_FLAG_BIT_EX_MACRO, c
		
		jr mic_ex_macro_done

	mic_no_ex_macro:

		inc hl
		inc hl
		inc hl

	mic_ex_macro_done:

	; update flags
	ld (ix + channel.flags), c

	inc hl
	ret
