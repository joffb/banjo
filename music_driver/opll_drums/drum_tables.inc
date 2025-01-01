
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

music_set_default_opll_drum_pitches:

	; tom?
	ld a, 0x16
	out (OPLL_REG_PORT), a
	ld a, 0x20
	out (OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, 0x26
	out (OPLL_REG_PORT), a
	ld a, 0x05
	out (OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	; snare?
	ld a, 0x17
	out (OPLL_REG_PORT), a
	ld a, 0x50
	out (OPLL_DATA_PORT), a

	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, 0x27
	out (OPLL_REG_PORT), a
	ld a, 0x05
	out (OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	; kick?
	ld a, 0x18
	out (OPLL_REG_PORT), a
	ld a, 0xc0
	out (OPLL_DATA_PORT), a

	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, 0x28
	out (OPLL_REG_PORT), a
	ld a, 0x01
	out (OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ret