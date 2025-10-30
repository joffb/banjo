
; banjo sound driver
; Joe Kennedy 2024

music_calculate_volume: 

	ld a, channel.volume(ix)
	and a, #0xf

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

				; vol = vol - (15 - volume_macro_vol)
				; vol = vol + volume_macro_vol - 15
				add a, l
				sub a, #15

		mcv_no_volume_macro: 
		
		; master volume active?
		ld h, music_state.master_volume(iy)
		bit 7, h
		jr nz, mcv_no_master_volume

			; scale channel volume by master volume
			; preserve volume
			ld l, a

			; prepare master volume value
			; vol = vol - (15 - ((master_volume >> 3) & 0xf))
			; vol = vol + ((master_volume >> 3) & 0xf) - 15
			ld a, h
			rrca 
			rrca 
			rrca 
			add a, l
			sub a, #15

		mcv_no_master_volume: 

		; is `a` negative now?
		or a, a
		
		; it's not negative, return it
		ret p

			; it is negative, set to 0
			xor a, a
			ret 

