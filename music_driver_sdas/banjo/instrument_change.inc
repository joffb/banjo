
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
	ld channel.instrument_num(ix), a
	
	; get instrument pointer by doing instrument_num * 12
	ld l, a
	ld h, #0
	
	; * 4
	add hl, hl
	add hl, hl
	
	; keep * 4 for later
	ld b, h
	ld c, l
	
	; * 8
	add hl, hl
	
	; * 12
	add hl, bc
	
	; offset into instrument data
	ld c, music_state.instrument_ptrs(iy)
	ld b, music_state.instrument_ptrs+1(iy)
	add hl, bc

	; get channel struct ptr ix into de
	push ix
	pop de

	; get channel.flags (should be first byte of channel struct)
	; then unset the macro flags and store in b 
	ld a, (de)
	and a, #~(CHAN_FLAG_VOLUME_MACRO|CHAN_FLAG_EX_MACRO)
	ld b, a

	; max value of c so ldi instructions don't affect b
	ld c, #0xff

	; move along to volume macro 
	ld a, #channel.volume_macro_len
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
	ld channel.flags(ix), b

	; restore de and bc
	pop de
	pop bc
	
	ret 
