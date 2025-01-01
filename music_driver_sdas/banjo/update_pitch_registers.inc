
; banjo sound driver
; Joe Kennedy 2023

; get fnum for note into de
; b: channel.events
; hl: tone lookup table address
; ix: channel
; iy: state
music_calc_fnum: 

	push bc

	; keep channel flags in c
	ld c, channel.flags(ix)

	; keep ex_macro_type in d
	ld d, channel.ex_macro_type(ix)

	; check if we have a pitch slide going as
	; it takes precedence over arpeggios
	bit CHAN_FLAG_BIT_SLIDE, c
	jr nz, mcf_slide

		; total arpeggio offset
		ld e, #0

		; check if an arpeggio effect is running
		bit CHAN_FLAG_BIT_ARPEGGIO, c
		jr z, mcf_no_arpeggio_effect

			call music_update_arpeggio
			add a, e
			ld e, a

		mcf_no_arpeggio_effect: 

		; check if we have an arpeggio ex macro running
		bit MACRO_TYPE_BIT_ARP, d
		jr z, mcf_no_arpeggio_macro

			ld a, channel.ex_macro_val(ix)

			; if bit 7 is set, it's absolute
			bit 7, a
			jr z, mcf_arp_macro_relative

				; absolute arp
				; isolate lower bits
				; add current arp and offset into table
				and a, #0x7f
				add a, e
				add a, a
				jr mcf_arpegggio_get_tone

			mcf_arp_macro_relative: 

				; relative arp
				; sign extend bit 6 into bit 7
				bit 6, a
				jr z, mcf_arp_rel_positive

					or a, #0x80

				mcf_arp_rel_positive: 
				add a, e
				ld e, a

		mcf_no_arpeggio_macro: 

			; get total arpeggio amount into a
			ld a, e

			; if it's zero, just use the freq we already have
			or a, a
			jr z, mcf_slide

			; add the channel's midi note to the arpeggio note
			; then double to make it an offset into the fnum table
			add a, channel.midi_note(ix)
			add a, a

		mcf_arpegggio_get_tone: 

			; add to tone lookup table address in hl
			add a, l
			ld l, a
			adc a, h
			sub a, l
			ld h, a

			ld a, (hl)
			inc hl
			ld h, (hl)
			ld l, a
			
			jr mcf_check_pitch_ex_macro

	mcf_slide: 

		; get tone fnum in hl
		ld l, channel.freq(ix)
		ld h, channel.freq+1(ix)

	; check if we have an arpeggio ex macro running
	mcf_check_pitch_ex_macro: 
	bit MACRO_TYPE_BIT_PITCH, d
	jr z, mcf_no_pitch_ex_macro

		; sign extend pitch amount into de
		ld a, channel.ex_macro_val(ix)
		ld e, a
		add a, a
		sbc a, a
		ld d, a

		; add to fnum
		add hl, de

	mcf_no_pitch_ex_macro: 
	
	; vibrato? 
	mcf_check_vibrato: 
	bit CHAN_FLAG_BIT_VIBRATO, c
	jr z, mcf_no_vibrato

		call music_update_vibrato

		; add to fnum
		add hl, de

	mcf_no_vibrato: 

	; get final pitch into de
	ex de, hl

	pop bc

	ret 
