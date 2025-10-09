
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
	
	; get instrument pointer by doing instrument_num * 10
	ld l, a
	ld h, #0
	
	; * 2
	add hl, hl
	; keep * 2 for later
	ld b, h
	ld c, l
	
	; * 8
	add hl, hl
	add hl, hl
	
	; * 10
	add hl, bc
	
	; offset into instrument data
	ld c, music_state.instrument_ptrs(iy)
	ld b, music_state.instrument_ptrs+1(iy)
	add hl, bc

	; get channel struct ptr ix into de
	push ix
	pop de

	; set macro flags
	; get channel.flags (should be first byte of channel struct)
	; then unset the macro flags and combine with flags from first byte of instrument 
	ld a, (de)
	and a, #~(CHAN_FLAG_VOLUME_MACRO|CHAN_FLAG_EX_MACRO)
	or a, (hl)
	ld channel.flags(ix), a

	; move to volume macro info
	inc hl

	; move along to volume_macro_ptr
	ld a, #channel.volume_macro_ptr
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	; a = 0
	xor a, a

	; volume macro
	; copy over macro info

	; ptr
	ldi 
	ldi 

	; move past ex_macro_val and ex_macro_pos
	inc de
	inc de

	; ex macro
	; copy over macro info

	; ex_macro_type
	ldi 

	; ex_macro_ptr
	ldi 
	ldi 

	; restore de and bc
	pop de
	pop bc
	
	ret 
