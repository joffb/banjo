
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

	; subtract master volume 
	; master volume should be an attenuation
	; a = channel volume - master_volume - volume macro value
	neg
	add a, (ix + channel.volume)

	; this will subtract the volume macro value
	; from the current volume value
    bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
	jp nz, music_update_volume_macro
	music_update_volume_macro_done:

	; we're done if the volume is positive
    ret p

		; zero it if the volume is negative
		xor a, a
		ret