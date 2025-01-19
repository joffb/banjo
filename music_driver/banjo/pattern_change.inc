
; banjo sound driver
; Joe Kennedy 2024

; reset all the channels' order and pattern pointers
; to the first order and pattern
; ix: channels
; iy: state
music_reset_channels_order_and_pattern:

    ; looping back to order 0
    ld (iy + music_state.order), 0

	push ix

	; get pointer to order_ptrs in hl
	ld l, (iy + music_state.order_ptrs)
	ld h, (iy + music_state.order_ptrs + 1)

	; loop through all channels
	ld b, (iy + music_state.channel_count)

	mrcoap_loop:
		
		; get address of first order into de
		; and update it in the channel
		ld e, (hl)
		ld (ix + channel.order_table_ptr), e
		inc hl
		ld d, (hl)
		ld (ix + channel.order_table_ptr + 1), d
		inc hl

		; using the order pointer, get the address
		; of the first pattern and update the channel
		ld a, (de)
		ld (ix + channel.pattern_ptr), a
		inc de
		ld a, (de)
		ld (ix + channel.pattern_ptr + 1), a

		; set line_wait = 0 for channel
		ld (ix + channel.line_wait), 0

		; move along to next channel
		ld de, _sizeof_channel
		add ix, de

		djnz mrcoap_loop

	pop ix

	ret


; move the order pointer along by one and update the pattern pointer
; to the pattern specified by the new order
; * relies on the channel struct being .order_table_ptr followed by .pattern_ptr 
; ix: channels
; iy: state
music_update_channels_order_and_pattern:

	ld b, (iy + music_state.channel_count)

    ; get channel pointer into hl
    push ix
    pop hl

    ; move it along to order_table_ptr
    ld de, channel.order_table_ptr
    add hl, de

	mucoap_loop:

		; get order pointer in de
		ld e, (hl)
        inc hl
		ld d, (hl)
        dec hl

		; advance to next order in table
		inc de
		inc de

		; store new order pointer in channel.order_table_ptr
		ld (hl), e
        inc hl
		ld (hl), d
        inc hl

		; get pattern pointer from (de) and store it in channel.pattern_ptr
		ld a, (de)
		ld (hl), a
		inc hl
        inc de
		ld a, (de)
		ld (hl), a

		; set line_wait = 0 for channel
		inc hl
		ld (hl), 0

		; move to next channel
		ld de, _sizeof_channel - 4
        add hl, de
		djnz mucoap_loop

	ret

; jump to a given order/pattern
; used for the 0x0b effect
; a: order number to jump to
; ix: channels
; iy: state
music_jump_channels_order_and_pattern:

	push ix

	ld b, (iy + music_state.channel_count)

	; update order index
	ld (iy + music_state.order), a

	; get pointer to first channel's orders in hl
	ld l, (iy + music_state.order_ptrs)
	ld h, (iy + music_state.order_ptrs + 1)

	; double order number
	add a, a

	; add pointer to first order offset by doubled order number
	; and keep in e
	add a, (hl)
	ld e, a

	; get upper byte of address with offset
	inc hl
	adc a, (hl)
	sub a, e
	ld h, a

	; fully offset address is now in hl
	ld l, e

	mjcoap_loop:

		; update order ptr
		ld (ix + channel.order_table_ptr), l
		ld (ix + channel.order_table_ptr + 1), h

		; update pattern ptr
		ld e, (hl)
		inc hl
		ld d, (hl)
		dec hl

		ld (ix + channel.pattern_ptr), e
		ld (ix + channel.pattern_ptr + 1), d

		; move order ptr along to next channel's orders
		; by adding 2 * order count

		; lower byte
		ld a, (iy + music_state.orders_length)
		add a, a
		add a, l 
		ld l, a

		; upper byte
		adc a, h
		sub a, l
		ld h, a

		; set line_wait = 0 for channel
		ld (ix + channel.line_wait), 0

		; move along to next channel
		ld de, _sizeof_channel
		add ix, de

		djnz mjcoap_loop

	pop ix

	; reset order_jump to 0xff to indicate we don't need to jump
	ld a, 0xff
	ld (iy + music_state.order_jump), a

	; reset line and tic to 0
	xor a, a
	ld (iy + music_state.line), a
	ld (iy + music_state.tic), a
	ld (iy + music_state.subtic), a

	ret
	