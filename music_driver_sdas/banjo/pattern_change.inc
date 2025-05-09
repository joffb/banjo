
; banjo sound driver
; Joe Kennedy 2024

; jump to a given order/pattern
; used for the 0x0b effect
; a: order number to jump to
; ix: channels
; iy: state
music_jump_channels_order_and_pattern: 

	push ix

	; de = orders_length * 2
	ld l, music_state.orders_length(iy)
	ld h, #0
	add hl, hl
	ex de, hl

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

	; loop through all channels
	ld b, music_state.channel_count(iy)

	mjcoap_loop: 

		; set line_wait = 0 for channel
		ld channel.line_wait(ix), #0

		; update pattern ptr
		ld a, (hl)
		ld channel.pattern_ptr(ix), a
		inc hl
		ld a, (hl)
		ld channel.pattern_ptr+1(ix), a
		dec hl

		; move to next channel's order
		add hl, de

		; preserve de
		push de

		; move along to next channel
		ld de, #_sizeof_channel
		add ix, de

		; restore de
		pop de

		djnz mjcoap_loop

	pop ix

	; reset order_jump to 0xff to indicate we don't need to jump
	ld a, #0xff
	ld music_state.order_jump(iy), a

	; reset line and tic
	ld a, music_state.pattern_length(iy)
	ld music_state.line(iy), a

	ld a, music_state.speed_1(iy)
	ld music_state.tic(iy), a

	ret 
	
