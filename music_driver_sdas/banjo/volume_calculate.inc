
; banjo sound driver
; Joe Kennedy 2024

; a : volume
; c : max volume
music_calculate_volume: 

	and a, c

	; if channel volume is 0 we can finish now
	ret z

		; volume macro active?
		bit CHAN_FLAG_BIT_VOLUME_MACRO, channel.flags(ix)
		jr z, mcv_no_volume_macro

			; preserve volume
			ld l, a

			; if macro volume is 0 we can finish now
			ld a, channel.volume_macro_vol(ix)
			or a, a
			ret z

				; vol = vol - (max - volume_macro_vol)
				; vol = vol + volume_macro_vol - max
				add a, l
				sub a, c

		mcv_no_volume_macro: 
		
		; master volume active? (bit 7 set)
		ld h, music_state.master_volume(iy)
		bit 7, h
		jr nz, mcv_no_master_volume

			; scale channel volume by master volume
			; prepare master volume value
			; vol = vol - (max - ((master_volume >> shift)))
			; vol = vol + ((master_volume >> shift)) - max

			; if max_volume == 0x7f then don't shift
			bit 6, c
			jr nz, mcv_mvv_no_shift

				srl h

			; if max_volume == 0x3f we only shift once
			bit 5, c
			jr nz, mcv_mvv_no_shift

				srl h
				srl h

			mcv_mvv_no_shift: 

			add a, h
			add a, l
			sub a, c

		mcv_no_master_volume: 

		; is `a` negative now?
		or a, a
		
		; it's not negative, return it
		ret p

			; it is negative, set to 0
			xor a, a
			ret 

