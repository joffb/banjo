
; banjo sound driver
; Joe Kennedy 2024

; include full opll code
.ifdef INCLUDE_OPLL

; OPLL Pitch slides
; slide type == 1, upward slide
music_update_fm_pitch_slide_upward: 

	; add slide_amount to lower byte
	ld a, e
	add a, channel.slide_amount(ix)
	ld e, a

	; add carry to upper byte
	adc a, d
	sub a, e
	ld d, a

	; we want to check if fnum > 325

	; isolate bit 8 of fnum
	and a, #0x1
	; combine with low byte of fnum - we don't care about the lowest bit
	or a, e
	; rotate so a now contains top 8 bits of the 9-bit fnum
	rrca 

	; a now contains the fnum / 2
	; check if a > (325 / 2)
	cp a, #162
	jp c, music_update_pitch_slide_done

		; a already contains half of the fnum so we can load that into e
		ld e, a

		; clear upper bit of fnum (lowest bit of d) and add 1 to block
		ld a, d
		and a, #0xfe
		add a, #0x2
		ld d, a

		; has block overflowed?
		and a, #0xf0
		jp z, music_update_pitch_slide_done

			; cap pitch
			ld de, #3990
			jp music_update_pitch_slide_done


; slide type == 2, downward slide
music_update_fm_pitch_slide_downward: 

	; sub slide_amount from lower byte
	ld a, e
	sub a, channel.slide_amount(ix)
	ld e, a

	; sub carry from upper byte
	ld a, d
	sbc a, e
	add a, e
	ld d, a

	; isolate bit 8 of fnum
	and a, #0x1
	; combine with low byte of fnum - we don't care about the lowest bit
	or a, e
	; rotate so a now contains top 8 bits of the 9-bit fnum
	rrca 

	; a now contains the fnum / 2
	; check if a < (120 / 2)
	cp a, #60
	jp nc, music_update_pitch_slide_done

		; double the fnum
		sla e
		sla e

		; subtract 1 from the block
		ld a, d
		and a, #0xfe
		sub a, #0x2
		ld d, a

		; has block underflowed?
		jp nc, music_update_pitch_slide_done

			; if it has, cap pitch
			ld de, #110
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
	ld l, channel.target_freq(ix)
	ld h, channel.target_freq+1(ix)

	; subtract current freq from target_freq
	sbc hl, de
	jr z, mufm_porta_end_portamento
	jr c, mufm_porta_current_higher

	mufm_porta_current_lower: 

		; get target_freq back into hl
		add hl, de

		; add slide amount to freq
		ld a, channel.slide_amount(ix)
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
        cp a, #352-256
        jr nc, mufm_porta_lower_octave_ok

            ; halve the fnum
            ; or-ing in the 256 bit from the upper byte as a 128 bit
            srl a
            or a, #0x80
            ld e, a
            
            ; clear the 256 bit from the fnum in the upper byte
            ld a, d
            and a, #0xfe
            ; add 1 to the octave
            add a, #0x2
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
		sub a, channel.slide_amount(ix)
		ld e, a
		ld a, d
		sbc a, #0
		ld d, a

        ; check if fnum < (fnum for note C) - some leeway
        ; if it is, we need to dec the octave and double the fnum
        ; if bit 0 of d is set, the fnum is > 256
        bit 0, d
        jr nz, mufm_porta_higher_octave_ok

        ; check the lower byte
        ld a, e
        cp a, #160
        jr nc, mufm_porta_higher_octave_ok

            ; double the fnum
            add a, a
            ld e, a
            adc a, d
            sub a, e
            ; subtract 1 from the octave
            sub a, #0x2
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
		ld a, #0
		ld channel.slide_type(ix), a

	mufm_porta_done: 

	    jp music_update_pitch_slide_done

; don't include full opll code
.else 

music_update_fm_pitch_slide_upward: 
music_update_fm_pitch_slide_downward: 
music_update_fm_portamento: 
    jp music_update_pitch_slide_done

.endif 
