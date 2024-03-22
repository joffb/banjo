
; banjo sound driver
; Joe Kennedy 2024

; ix: current channel
; iy: music state
; de: current channel.freq
; outputs updated freq in de
music_update_vibrato_sn:
	
	; add counter_add to counter and update variable
	; ensure the value we load is between 0-15 by doing % 16
	ld a, (ix + channel.vibrato_counter)
	and a, 0xf
	add a, (ix + channel.vibrato_counter_add)
	ld (ix + channel.vibrato_counter), a

	; is the new counter value < 16? if so, we're done for now
	cp a, 16
	jp c, muvib_done

		; counter has "overflowed"

		; get amount to add by rotating top bits of counter into lowest bits
		; store in hl for now
		rlca
		rlca
		rlca
		rlca
		and a, 0xf
		ld l, a
		ld h, 0

		; get target vibrato amplitude into c
		ld c, (ix + channel.vibrato_target)

		; check whether target is > VIBRATO_CENTRE to decide vibrato direction
		ld a, VIBRATO_CENTRE
		cp a, c
		jr nc, musnv_neg

			; for sn, subtract from current frequency to increase pitch
			or a, a
			ex de, hl
			sbc hl, de
			ex de, hl

			; add to current vibrato amplitude and save it
			ld a, (ix + channel.vibrato_current)
			add a, l
			ld (ix + channel.vibrato_current), a

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
			ld a, (ix + channel.vibrato_current)
			sub a, l
			ld (ix + channel.vibrato_current), a

			; check if current <= target
			cp a, c
			jr z, musnv_target_reached
			jr c, musnv_target_reached
			jp muvib_done

	musnv_target_reached:

		; negate target amplitude around VIBRATO_CENTRE
		ld a, VIBRATO_CENTRE * 2
		sub a, c
		ld (ix + channel.vibrato_target), a

	jp muvib_done