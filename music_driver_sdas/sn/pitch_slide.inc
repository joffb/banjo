
; banjo sound driver
; Joe Kennedy 2024

; SN Pitch Slides

; slide type == 1, upward slide
music_update_sn_pitch_slide_upward: 

	; sub slide_amount from freq
	ld l, channel.slide_amount(ix)
	ld h, #0
	ex de, hl
	sbc hl, de

	; if carry, de has gone < 0, so cap the pitch
	jr nc, mu_sn_psu_done

		ld hl, #10

	; done
	mu_sn_psu_done: 
	ex de, hl
	jp music_update_pitch_slide_done

; slide type == 2, downward slide
music_update_sn_pitch_slide_downward: 

	; add slide_amount to freq
	ld l, channel.slide_amount(ix)
	ld h, #0
	add hl, de
	
	; check if h < 1024 so has gone beyond the bottom pitch
	ld a, h
	cp a, #4
	jr c, mu_sn_psd_done

		; cap pitch
		ld hl, #1017

	; done
	mu_sn_psd_done: 
	ex de, hl
	jp music_update_pitch_slide_done

; handle SN chip portamento
; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_sn_portamento: 

	; get target_freq in hl
	ld l, channel.target_freq(ix)
	ld h, channel.target_freq+1(ix)

	; subtract current freq from target_freq
	sbc hl, de
	jr z, musn_porta_end_portamento
	jr c, musn_porta_current_higher

	musn_porta_current_lower: 

		; get target_freq back into hl
		add hl, de

		; add slide amount to freq
		ld a, channel.slide_amount(ix)
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a

		; check if we've passed target_freq
		; if z then freq == target and we're done
		; if c then freq > target and we're done
		; if nc then we carry on next loop
		sbc hl, de
		jr z, musn_porta_end_portamento
		jr c, musn_porta_end_portamento
		jr nc, musn_porta_done

	musn_porta_current_higher: 

		; get target_freq back into hl
		add hl, de

		; sub slide amount from freq
		ld a, e
		sub a, channel.slide_amount(ix)
		ld e, a
		ld a, d
		sbc a, #0
		ld d, a

		; check if we've passed target_freq
		; if z then freq == target and we're done
		; if nc then freq < target and we're done
		; if nc then we carry on next loop
		sbc hl, de
		jr z, musn_porta_end_portamento
		jr nc, musn_porta_end_portamento
		jr c, musn_porta_done

	musn_porta_end_portamento: 

		; restore target_freq into hl
		; exhange target_freq into de
		add hl, de
		ex de, hl
			
		; cancel the portamento
		ld a, #0
		ld channel.slide_type(ix), a

	musn_porta_done: 

		jp music_update_pitch_slide_done
