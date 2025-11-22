
; banjo sound driver
; Joe Kennedy 2024

; a : volume
; c : max volume
music_calculate_volume:

	and a, c

	; if channel volume is 0 we can finish now
	ret z

		; volume macro active?
		bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
		jr z, mcv_no_volume_macro

			; volume macro should be like an attenuation
			sub a, (ix + channel.volume_macro_vol)

			; zero means we're done
			ret z

			; continue if volume is still positive
			jp p, mcv_vm_not_neg

				; macro has made the volume go negative
				; zero it out
				xor a, a
				ret
		
		mcv_vm_not_neg:
		mcv_no_volume_macro:
		
		; master volume active? (bit 7 set)
		ld l, (iy + music_state.master_volume)
		bit 7, l
		jr nz, mcv_no_master_volume

			; scale channel volume by master volume
			; prepare master volume value
			; vol = vol - (max - ((master_volume >> shift)))
			; vol = vol + ((master_volume >> shift)) - max

			; if max_volume == 0x7f then don't shift
			bit 6, c
			jr nz, mcv_mvv_no_shift

				srl l

			; if max_volume == 0x3f we only shift once
			bit 5, c
			jr nz, mcv_mvv_no_shift

				srl l
				srl l

			mcv_mvv_no_shift:

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

