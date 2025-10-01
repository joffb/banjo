
; banjo sound driver
; Joe Kennedy 2024

; jump to a given order/pattern
; used for the 0x0b effect
; a: order number to jump to
; ix: channels
; iy: state
music_jump_channels_order_and_pattern: 

	; update order index
	ld music_state.order(iy), a

	; double order number in hl
	ld h, #0
	ld l, a
	add hl, hl

	; get pointer to orders list in bc
	ld c, music_state.order_ptrs(iy)
	ld b, music_state.order_ptrs+1(iy)

	; add them together
	; address of this order for the first channel is now pointed at by hl
	add hl, bc

	; get pointer to order in hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; loop through all channels
	ld b, music_state.channel_count(iy)
	ld c, #0xff

	; point de at first channel's channel.pattern_ptr 
	push ix
	pop de
	
	ld a, #channel.pattern_ptr
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	mjcoap_loop: 

		; update pattern ptr
		ldi 
		ldi 

		; set line_wait = 0 for channel
		xor a, a
		ld (de), a

		; move along to next channel
		ld a, #_sizeof_channel-2
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a

		djnz mjcoap_loop

	; reset order_jump to 0xff to indicate we don't need to jump
	ld a, #0xff
	ld music_state.order_jump(iy), a

	; reset line and tic
	ld a, music_state.pattern_length(iy)
	ld music_state.line(iy), a

	ld a, music_state.speed_1(iy)
	ld music_state.tic(iy), a

	ret 
	
