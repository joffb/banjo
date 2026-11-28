
; banjo sound driver
; Joe Kennedy 2023


.ifndef BANJO_MINIMAL

; get fnum for note into de
; b: channel.events
; ix: channel
; iy: state
music_calc_fnum:

	; slide active?
	bit CHAN_EVENT_BIT_SLIDE, b
	jr z, mnc_no_slide

		; update slide, note is now in hl
		call music_update_slide
		jr mnc_slide_done

	mnc_no_slide:

		; get note into hl
		ld l, (ix + channel.freq)
		ld h, (ix + channel.freq + 1)

		.ifndef BANJO_DISABLE_ARPEGGIO

		; arp effect active?
		bit CHAN_EVENT_BIT_ARPEGGIO, b
		jr z, mnc_no_arp

			; run arpeggio and add it to the note part of the fnum
			call music_update_arpeggio
			add a, h
			ld h, a

		mnc_no_arp:

		.endif

		; arp macro active?
		bit CHAN_EVENT_BIT_ARP_MACRO, b
		jr z, mnc_no_arp_macro

			; preserve current pitch
			push hl

			; get macro position and pointer
			; then run macro update
			ld c, (ix + channel.arp_macro_pos)
			ld e, (ix + channel.arp_macro_ptr)
			ld d, (ix + channel.arp_macro_ptr + 1)

			call music_macro_update

			; save new macro position
			ld (ix + channel.arp_macro_pos), c

			; get arp macro val
			ld a, (hl)

			; restore current pitch
			pop hl

			; check if arp macro val is absolute
			bit 7, a
			jr nz, mnc_arp_macro_absolute

				; relative arp macro
				; sign extend bit 6 into bit 7
				rla
				sra a

				; add arp macro val to the note part of the fnum
				add a, h
				ld h, a

				jr mnc_arp_macro_done

			mnc_arp_macro_absolute:
				
				; absolute arp macro
				; set note part of the fnum to the add arp macro val
				and a, 0x7f
				ld h, a

		mnc_no_arp_macro:
		mnc_arp_macro_done:

		.ifndef BANJO_DISABLE_VIBRATO

		; vibrato active?
		bit CHAN_EVENT_BIT_VIBRATO, b
		jr z, mnc_no_vibrato

			; update vibrato and add to fnum
			call music_update_vibrato

		mnc_no_vibrato:

		.endif

	mnc_slide_done:

	; return fnum in de
	ex de, hl

	ret

.endif

.ifdef BANJO_MINIMAL

; get fnum for note into de
; b: channel.events
; hl: tone lookup table address
; ix: channel
; iy: state
music_calc_fnum:

	; add the channel's midi note to the arpeggio note
	; then double to make it an offset into the fnum table
	ld a, (ix + channel.freq)
	add a, a

	; add to tone lookup table address in hl
	add a, l
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	ld e, (hl)
	inc hl
	ld d, (hl)

	ret

.endif