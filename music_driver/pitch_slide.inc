
; banjo sound driver
; Joe Kennedy 2023

; update the pitch slides or portamento for a channel
; ix: channel
; iy: state
music_update_pitch_slide:

	; flag that we need to update the pitch
	ld a, (ix + channel.flags)
	or a, CHAN_FLAG_PITCH_CHANGED
	ld (ix + channel.flags), a

	; get current freq into de
	ld e, (ix + channel.freq)
	ld d, (ix + channel.freq + 1)

	; check whether this is an SN or OPLL channel
	ld a, (ix + channel.type)
	or a, a 
	jr z, music_update_sn_pitch_slide
	jr music_update_fm_pitch_slide

	music_update_pitch_slide_done:

	; update freq with new value
	ld (ix + channel.freq), e
	ld (ix + channel.freq + 1), d

	jp mpct_check_arpeggio
	
; SN Pitch Slide
music_update_sn_pitch_slide:

	; check if this is a portamento
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jp z, music_update_sn_portamento

	; slide type == 1, upward slide
	cp a, SLIDE_TYPE_UP
	jr nz, music_update_sn_pitch_slide_downward

	music_update_sn_pitch_slide_upward:

		; sub slide_amount from lower byte
		ld a, e
		sub a, (ix + channel.slide_amount)
		ld e, a

		; sub carry from upper byte
		ld a, d
		sbc a, e
		add a, e
		ld d, a

		; done
		jr music_update_pitch_slide_done

	; slide type == 2, downward slide
	music_update_sn_pitch_slide_downward:

		; add slide_amount to lower byte
		ld a, e
		add a, (ix + channel.slide_amount)
		ld e, a

		; add carry to upper byte
		adc a, d
		sub a, e
		ld d, a

		; done
		jr music_update_pitch_slide_done

; OPLL Pitch slide
music_update_fm_pitch_slide:

	; check if it's portamento
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jp z, music_update_fm_portamento

	; slide type == 1, upward slide
	cp a, SLIDE_TYPE_UP
	jr nz, music_update_fm_pitch_slide_downward

	music_update_fm_pitch_slide_upward:

		; get current upper byte and isolate the octave bits, store them in c
		ld a, d
		and a, 0xe
		ld c, a

		; add slide_amount to lower byte
		ld a, e
		add a, (ix + channel.slide_amount)
		ld e, a

		; add carry to upper byte
		adc a, d
		sub a, e
		and a, 0xf
		ld d, a

		; check if the octave bits have changed as the fnum gets > 511
		and a, 0xe
		cp a, c
		jr z, music_update_pitch_slide_done

			; if the octave has changed, we add 256 to the fnum as it's effectively 512
			; but we just went up an octave, so we should halve the fnum to 256
			srl e
			ld a, d
			or a, 0x1
			ld d, a

			; done
			jr music_update_pitch_slide_done

	; slide type == 2, downward slide
	music_update_fm_pitch_slide_downward:

		; sub slide_amount from lower byte
		ld a, e
		sub a, (ix + channel.slide_amount)
		ld e, a

		; sub carry from upper byte
		ld a, d
		sbc a, e
		add a, e
		and a, 0xf
		ld d, a

		; get 7th, 8th and 9th bit of fnum in a byte to see if the fnum is < 128
		and a, 0x1
		ld c, a
		ld a, e
		and a, 0x80
		or a, c

		; if it's zero, we're < 128
		jr nz, music_update_pitch_slide_done

			; lower the octave and double the fnum to go from 128 on one octave
			; to 256 on the other to give some room to descend further
			ld a, d
			sub a, 1 << 1
			and a, 0xf
			ld d, a
			sla e

			; done
			jr music_update_pitch_slide_done


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

		jp music_update_pitch_slide_done


; handle OPLL portamento
;   note that as the ranges of fnums we're using for the notes C to B span from
;   C = 172 to B = 325
;   we need to move to the next octave when we go too far above/below those fnums
;   otherwise we'll never hit the target_freq's combination of octave and fnum
; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_fm_portamento:
    
	; get target_freq in hl
	ld l, (ix + channel.target_freq)
	ld h, (ix + channel.target_freq + 1)

	; subtract current freq from target_freq
	sbc hl, de
	jr z, mufm_porta_end_portamento
	jr c, mufm_porta_current_higher

	mufm_porta_current_lower:

		; get target_freq back into hl
		add hl, de

		; add slide amount to freq
		ld a, (ix + channel.slide_amount)
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a

        ; check if fnum > (fnum for note B) + leeway
        ; if it is, we need to inc the octave and half the fnum

        ; if bit 0 of d isn't set, the fnum is < 256
        bit 0, d
        jr z, mufm_porta_lower_octave_ok

        ; check if the lower byte makes the full fnum > 352
        ld a, e
        cp a, 352 - 256
        jr nc, mufm_porta_lower_octave_ok

            ; halve the fnum
            ; or-ing in the 256 bit from the upper byte as a 128 bit
            srl a
            or a, 0x80
            ld e, a
            
            ; clear the 256 bit from the fnum in the upper byte
            ld a, d
            and a, 0xfe
            ; add 1 to the octave
            add a, 0x2
            ld d, a

        mufm_porta_lower_octave_ok:
		; check if we've passed target_freq
		; if z then freq == target and we're done
		; if c then freq > target and we're done
		; if nc then we carry on next loop
		sbc hl, de
		jr z, mufm_porta_end_portamento
		jr c, mufm_porta_end_portamento
		jr nc, mufm_porta_done

	mufm_porta_current_higher:

		; get target_freq back into hl
		add hl, de

		; sub slide amount from freq
		ld a, e
		sub a, (ix + channel.slide_amount)
		ld e, a
		ld a, d
		sbc a, 0
		ld d, a

        ; check if fnum < (fnum for note C) - some leeway
        ; if it is, we need to dec the octave and double the fnum
        ; if bit 0 of d is set, the fnum is > 256
        bit 0, d
        jr nz, mufm_porta_higher_octave_ok

        ; check the lower byte
        ld a, e
        cp a, 160
        jr nc, mufm_porta_higher_octave_ok

            ; double the fnum
            add a, a
            ld e, a
            adc a, d
            sub a, e
            ; subtract 1 from the octave
            sub a, 0x2
            ld d, a

        mufm_porta_higher_octave_ok:
		; check if we've passed target_freq
		; if z then freq == target and we're done
		; if nc then freq < target and we're done
		; if nc then we carry on next loop
		sbc hl, de
		jr z, mufm_porta_end_portamento
		jr nc, mufm_porta_end_portamento
		jr c, mufm_porta_done

	mufm_porta_end_portamento:

		; restore target_freq into hl
		; exhange target_freq into de
		add hl, de
		ex de, hl
			
		; cancel the portamento
		ld a, 0
		ld (ix + channel.slide_type), a

	mufm_porta_done:

	    jp music_update_pitch_slide_done
	
