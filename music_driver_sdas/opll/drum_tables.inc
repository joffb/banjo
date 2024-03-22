
; banjo sound driver
; Joe Kennedy 2023

; include full opll code
.ifdef INCLUDE_OPLL

; bits which correspond to noteons in rhythm register
fm_drum_triggers: 
	; kick
	.db 0x10
	; snare
	.db 0x08
	; tom
	.db 0x04
	; cymbal
	.db 0x02
	; hi-hat
	.db 0x01

; registers for each rhythm channel (snare/hat and tom/cymbal share)
fm_drum_pitch_registers: 
	; kick
	.db 0x16
	; snare
	.db 0x17
	; tom
	.db 0x18
	; cymbal
	.db 0x18
	; hi-hat
	.db 0x17

; masks used to preserve volumes for other drums which share the byte
fm_drum_volume_masks: 
	; kick
	.db 0xf0
	; snare
	.db 0xf0
	; tom
	.db 0x0f
	; cymbal
	.db 0xf0
	; hi-hat
	.db 0x0f

music_set_default_fm_drum_pitches: 

	; tom?
	ld a, #0x16
	out (#OPLL_REG_PORT), a
	ld a, #0x20
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, #0x26
	out (#OPLL_REG_PORT), a
	ld a, #0x05
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	; snare?
	ld a, #0x17
	out (#OPLL_REG_PORT), a
	ld a, #0x50
	out (#OPLL_DATA_PORT), a

	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, #0x27
	out (#OPLL_REG_PORT), a
	ld a, #0x05
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	; kick?
	ld a, #0x18
	out (#OPLL_REG_PORT), a
	ld a, #0xc0
	out (#OPLL_DATA_PORT), a

	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ld a, #0x28
	out (#OPLL_REG_PORT), a
	ld a, #0x01
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	ret 


; don't include full opll code
.else 

music_set_default_fm_drum_pitches: 
	ret 
	
fm_drum_triggers: 
fm_drum_pitch_registers: 
fm_drum_volume_masks: 

.endif 
