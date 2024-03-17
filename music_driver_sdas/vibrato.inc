
; banjo sound driver
; Joe Kennedy 2024

muvib_jump_table: 

    jp music_update_vibrato_sn
    jp music_update_vibrato_fm
    jp muvib_done

; ix: current channel
; iy: music state
music_update_vibrato: 

	; flag that we need to update the pitch
	set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

    ; get channel frequency into de
    ld e, channel.freq(ix)
    ld d, channel.freq+1(ix)

    ; get address in jump table for this channel type in hl
    ; lower byte
    ld a, channel.type(ix)
    ld c, a
    add a, a
    add a, c
    add a, #<muvib_jump_table
    ld l, a

    ; upper byte
    adc a, #>muvib_jump_table
    sub a, l
    ld h, a

    ; jump into jump table
    jp (hl)

    muvib_done: 

        ; update channel frequency
        ld channel.freq(ix), e
        ld channel.freq+1(ix), d

        ret 

; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_vibrato_sn: 
	
	; add counter_add to counter and update variable
	; ensure the value we load is between 0-15 by doing % 16
	ld a, channel.vibrato_counter(ix)
	and a, #0xf
	add a, channel.vibrato_counter_add(ix)
	ld channel.vibrato_counter(ix), a

	; is the new counter value < 16? if so, we're done for now
	cp a, #16
	jp c, muvib_done

		; counter has "overflowed"

		; get amount to add by rotating top bits of counter into lowest bits
		; store in hl for now
		rlca 
		rlca 
		rlca 
		rlca 
		and a, #0xf
		ld l, a
		ld h, #0

		; get target vibrato amplitude into c
		ld c, channel.vibrato_target(ix)

		; check whether target is > VIBRATO_CENTRE to decide vibrato direction
		ld a, #VIBRATO_CENTRE
		cp a, c
		jr nc, musnv_neg

			; for sn, subtract from current frequency to increase pitch
			or a, a
			ex de, hl
			sbc hl, de
			ex de, hl

			; add to current vibrato amplitude and save it
			ld a, channel.vibrato_current(ix)
			add a, l
			ld channel.vibrato_current(ix), a

			; check if current >= target
			cp a, c
			jp nc, musnv_target_reached
			jp muvib_done

		musnv_neg: 

			; for sn, add to current frequency to decrease pitch
			ex de, hl
			add hl, de
			ex de, hl

			; sub from vibrato amplitude and save it
			ld a, channel.vibrato_current(ix)
			sub a, l
			ld channel.vibrato_current(ix), a

			; check if current <= target
			cp a, c
			jr z, musnv_target_reached
			jr c, musnv_target_reached
			jp muvib_done

	musnv_target_reached: 

		; negate target amplitude around VIBRATO_CENTRE
		ld a, #VIBRATO_CENTRE*2
		sub a, c
		ld channel.vibrato_target(ix), a

	jp muvib_done

; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_vibrato_fm: 
	
	; add counter_add to counter and update variable
	; ensure the value we load is between 0-15 by doing % 16
	ld a, channel.vibrato_counter(ix)
	and a, #0xf
	add a, channel.vibrato_counter_add(ix)
	ld channel.vibrato_counter(ix), a

	; is the new counter value < 16? if so, we're done for now
	cp a, #16
	jp c, muvib_done

		; counter has "overflowed"

		; get amount to add by rotating top bits of counter into lowest bits
		; store in hl for now
		rlca 
		rlca 
		rlca 
		rlca 
		and a, #0xf
		ld l, a
		ld h, #0

		; get target vibrato amplitude into c
		ld c, channel.vibrato_target(ix)

		; check whether target is > VIBRATO_CENTRE to decide vibrato direction
		ld a, #VIBRATO_CENTRE
		cp a, c
		jr nc, mufmv_neg

			; add to current frequency
			ex de, hl
			add hl, de
			ex de, hl

			; add to current vibrato amplitude and save it
			ld a, channel.vibrato_current(ix)
			add a, l
			ld channel.vibrato_current(ix), a

			; check if current >= target
			cp a, c
			jr nc, mufmv_target_reached
			jp muvib_done

		mufmv_neg: 

			; subtract from current frequency
			or a, a
			ex de, hl
			sbc hl, de
			ex de, hl

			; sub from current vibrato amplitude and save it
			ld a, channel.vibrato_current(ix)
			sub a, l
			ld channel.vibrato_current(ix), a

			; check if current <= target
			cp a, c
			jr z, mufmv_target_reached
			jr c, mufmv_target_reached
			jp muvib_done

	mufmv_target_reached: 

		; negate target amplitude around VIBRATO_CENTRE
		ld a, #VIBRATO_CENTRE*2
		sub a, c
		ld channel.vibrato_target(ix), a

	jp muvib_done
