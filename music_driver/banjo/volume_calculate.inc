
; banjo sound driver
; Joe Kennedy 2024

; used for chips with volume range 0-f
; a : master volume
music_calculate_volume_0x0f:

	srl a

; used for chips with volume range 0-1f
; a : master volume
music_calculate_volume_0x1f:

	srl a
	
; used for chips with volume range 0-3f
; a : master volume
music_calculate_volume_0x3f:

	srl a

; used for chips with volume range 0-7f
; a : master volume
music_calculate_volume_0x7f:

	; master volume should be negative as it's an attenuation	
	neg

	; this will subtract the volume macro value
	; from the current volume value
    bit CHAN_EVENT_BIT_VOLUME_MACRO, (ix + channel.events)
	jr z, mcv_no_volume_macro

		push af

		; get macro position and pointer
		; then run macro update
		ld c, (ix + channel.volume_macro_pos)
		ld e, (ix + channel.volume_macro_ptr)
		ld d, (ix + channel.volume_macro_ptr + 1)

		call music_macro_update

		; save new macro position
		ld (ix + channel.volume_macro_pos), c

		pop af

		; subtract macro value (should be pointed at by hl)
		sub a, (hl)

	mcv_no_volume_macro:

	; a = channel volume - master_volume - volume macro value
	add a, (ix + channel.volume)

	; we're done if the volume is positive
    ret p

		; zero it if the volume is negative
		xor a, a
		ret