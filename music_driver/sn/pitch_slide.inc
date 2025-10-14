
; banjo sound driver
; Joe Kennedy 2024

; sn slide type jump table
music_update_pitch_slide_sn_jump_table:

	jr music_update_pitch_slide_done_sn
	jr music_update_sn_pitch_slide_upward
	jr music_update_sn_pitch_slide_downward 
	jr music_update_sn_portamento

; update the pitch slides or portamento for a channel
; ix: channel
; iy: state
music_update_pitch_slide_sn:

	; flag that we're updating the pitch
	set CHAN_EVENT_BIT_PITCH_CHANGED, (ix + channel.events)

	; get current freq into de
	ld e, (ix + channel.freq)
	ld d, (ix + channel.freq + 1)

	; offset into jump table by slide type * 2
	ld a, (ix + channel.slide_type)
	add a, a
	add a, <music_update_pitch_slide_sn_jump_table
	ld l, a
	adc a, >music_update_pitch_slide_sn_jump_table
	sub a, l
	ld h, a

	; jump to function for this slide type
	jp (hl)

music_update_pitch_slide_done_sn:

	; update freq with new value
	ld (ix + channel.freq), e
	ld (ix + channel.freq + 1), d

	ret

; slide type == 1, upward slide
music_update_sn_pitch_slide_upward:

	; sub slide_amount from freq
	ld l, (ix + channel.slide_amount)
	ld h, 0
	ex de, hl
	sbc hl, de

	; if carry, de has gone < 0, so cap the pitch
	jr nc, mu_sn_psu_done

		ld hl, 0

	; done
	mu_sn_psu_done:
	ex de, hl
	jp music_update_pitch_slide_done_sn

; slide type == 2, downward slide
music_update_sn_pitch_slide_downward:

	; add slide_amount to freq
	ld l, (ix + channel.slide_amount)
	ld h, 0
	add hl, de

	; is freq > 1017?
	ld de, 1017
	sbc hl, de
	jr c, mu_sn_psd_done

		; cap pitch at 1017
		ld hl, 0

	; done
	mu_sn_psd_done:
	add hl, de

	ex de, hl
	jp music_update_pitch_slide_done_sn

; handle SN chip portamento
; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_sn_portamento:

	; get target_freq in hl
	ld l, (ix + channel.target_freq)
	ld h, (ix + channel.target_freq + 1)

	; subtract current freq from target_freq
	sbc hl, de
	jr z, musn_porta_end_portamento
	jr c, musn_porta_current_higher

	musn_porta_current_lower:

		; get target_freq back into hl
		add hl, de

		; add slide amount to freq
		ld a, (ix + channel.slide_amount)
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
		sub a, (ix + channel.slide_amount)
		ld e, a
		ld a, d
		sbc a, 0
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
		ld a, 0
		ld (ix + channel.slide_type), a

	musn_porta_done:

		jp music_update_pitch_slide_done_sn