
; banjo sound driver
; Joe Kennedy 2023

; ix: channel
; iy: state
; a: new instrument number
music_instrument_change:

	; preserve de
	push de

	; store the new instrument number in the channel
	ld (ix + channel.instrument_num), a
	
    .assert _sizeof_instrument == 12   ; ensure that an instrument is the size we think it is!

	; get instrument pointer by doing instrument_num * 12
	ld l, a
	ld h, 0
	
	; * 4
	add hl, hl
	add hl, hl

	; keep * 4 for later
	ld d, h
	ld e, l
	
	; * 8
	add hl, hl
	
	; * 12
	add hl, de
	
	; offset into instrument data
	ld e, (iy + music_state.instrument_ptrs)
	ld d, (iy + music_state.instrument_ptrs + 1)
	add hl, de

	; set macro flags in flags byte
	ld a, c
	and a, ~(CHAN_FLAG_EX_MACRO)
	or a, (hl)
	ld c, a
	
	inc hl 

	; set macro flags in events byte
	ld a, b
	and a, ~(CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO)
	or a, (hl)
	ld b, a

	; preserve bc
	push bc

	; move to volume macro info in instrument
	inc hl

	; get channel struct ptr ix into de
	; pointing at volume_macro_ptr
	ld a, channel.volume_macro_ptr
	add a, ixl
	ld e, a
	adc a, ixh
	sub a, e
	ld d, a

	; copy over volume macro ptr
	ldi
	ldi
	; copy over arp macro ptr
	ldi
	ldi
	; copy over ex macro ptr
	ldi
	ldi

	mic_run_call:

	; get pointer to extended instrument data
	ld e, (hl)
	inc hl
	ld d, (hl)
	inc hl

	; get pointer to extended instrument call
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; jump to extended instrument call
	jp (hl)

	banjo_instrument_change_done:

	; restore bc and de
	pop bc
	pop de
	
	ret


; used to e.g. restore FM patches after unmuting
; ix: channel
; iy: state
music_instrument_change_run_instrument_call:

	; get instrument number
	ld a, (ix + channel.instrument_num)

	; don't do anything if the instrument is unset (== 0xff)
	cp a, 0xff
	ret z

	; preserve de and bc
	push de
	push bc

	; get instrument number in hl
	ld l, a
	ld h, 0

    .assert _sizeof_instrument == 12   ; ensure that an instrument is the size we think it is!

	; * 4
	add hl, hl
	add hl, hl

	; keep * 4 for later
	ld d, h
	ld e, l
	
	; * 8
	add hl, hl
	
	; * 12
	add hl, de

	; offset to instrument call data pointer
	ld de, instrument.instrument_call_data
	add hl, de
	
	; offset into instrument data
	ld e, (iy + music_state.instrument_ptrs)
	ld d, (iy + music_state.instrument_ptrs + 1)
	add hl, de

	jp mic_run_call