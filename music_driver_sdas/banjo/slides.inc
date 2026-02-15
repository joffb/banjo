

; banjo sound driver
; Joe Kennedy 2025

music_update_slide: 

	; slide amount in hl
	ld h, #0
	ld l, channel.effect2(ix)

	; slide multiplier
	add hl, hl
	add hl, hl
	add hl, hl

	; slide amount in de
	ex de, hl

	; get current freq
	ld l, channel.freq(ix)
	ld h, channel.freq+1(ix)

	; check slide type 
	ld a, channel.effect1(ix)
	dec a
	jr nz, mus_not_up

		; slide up

		add hl, de

		; check if the slide has gone too high
		bit 7, h
		jr z, mus_up_freq_ok

			; cap value
			ld hl, #0x7f00

		mus_up_freq_ok: 
		jr mus_slide_done

	mus_not_up: 
	dec a
	jr nz, mus_not_down

		; slide down

		sbc hl, de

		; check if the slide has gone too low
		jr nc, mus_down_freq_ok

			; cap value
			ld hl, #0x0000

		mus_down_freq_ok: 
		jr mus_slide_done

	mus_not_down: 

		; portamento

		ld c, channel.target_freq(ix)

		; which direction? 
		ld a, h
		cp a, c
		jr nc, mus_porta_down

			; portamento up
			add hl, de

			; have we passed the target note?
			ld a, h
			cp a, c
			jr nc, mus_porta_target_passed
			jr mus_slide_done

		mus_porta_down: 

			; portamento down
			sbc hl, de
			
			; have we passed the target note?
			ld a, h
			cp a, c
			jr c, mus_porta_target_passed
			jr mus_slide_done

		mus_porta_target_passed: 

			ld l, #0
			ld h, c

			res CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	mus_slide_done: 

		ld channel.freq(ix), l
		ld channel.freq+1(ix), h

		ret 
