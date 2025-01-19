
; banjo sound driver
; Joe Kennedy 2023

; ix: channel
; iy: state
; a: new instrument number
music_instrument_change:

	; preserve bc and de
	push bc
	push de

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

	; get channel struct ptr ix into de
	push ix
	pop de

	; get channel.flags (should be first byte of channel struct)
	; then unset the macro flags and store in b 
	ld a, (de)
	and a, ~(CHAN_FLAG_VOLUME_MACRO | CHAN_FLAG_EX_MACRO)
	ld b, a

	; max value of c so ldi instructions don't affect b
	ld c, 0xff

	; move along to volume macro 
	ld a, channel.volume_macro_len
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	; a = 0
	xor a, a

	; volume macro
	; copy over macro info

	; volume macro length == 0?
	cp a, (hl)
	jr z, mic_no_volume_macro

		; set volume macro flag
		set CHAN_FLAG_BIT_VOLUME_MACRO, b
	
	mic_no_volume_macro:

		; len
		ldi

		; vol
		inc de

		; pos = 0
		ld (de), a
		inc de

		; loop
		ldi

		; ptr
		ldi
		ldi

	; ex macro
	; copy over macro info

	; always copy over ex_macro_type
	ldi

	; check if macro_len > 0
	cp a, (hl)
	jr z, mic_no_ex_macro

		; set volume macro flag
		set CHAN_FLAG_BIT_EX_MACRO, b

	mic_no_ex_macro:

		;len
		ldi

		; vol
		inc de

		; pos = 0
		ld (de), a
		inc de

		; loop
		ldi
		
		; ptr
		ldi
		ldi

	; update flags
	ld (ix + channel.flags), b

	; restore de and bc
	pop de
	pop bc
	
	ret
