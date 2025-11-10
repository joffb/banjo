
; banjo sound driver
; Joe Kennedy 2023

; for each rhythm channel
; * pitch register
; * drum trigger bit
; * volume mask to preserve volumes for other drums which share the byte
opll_drum_pitch_trigger_masks:

	; kick
	.db 0x16
	.db 0x10
	.db 0xf0

	; snare
	.db 0x17
	.db 0x08
	.db 0xf0

	; tom
	.db 0x18
	.db 0x04
	.db 0x0f

	; cymbal
	.db 0x18
	.db 0x02
	.db 0xf0

	; hi-hat
	.db 0x17
	.db 0x01
	.db 0x0f

opll_default_drum_pitches:

	; tom?
	.db 0x20
	.db 0x05

	.db 0x50
	.db 0x05

	.db 0xc0
	.db 0x01

music_set_default_opll_drum_pitches:

	ld hl, opll_default_drum_pitches
	ld b, 9
	ld c, OPLL_REG_PORT

	; start reg
	ld a, 0x16

	msdodp_loop:

		; set register
		out (c), a

		; send data
		inc c
		outi

		call banjo_opll_write_delay

		; move to next register
		add a, 0x10

		; set register
		dec c
		out (c), a

		; send data
		inc c
		outi

		call banjo_opll_write_delay

		; move back to register port
		dec c

		; move to next register
		sub a, 0x10 - 1

		djnz msdodp_loop

	ret

