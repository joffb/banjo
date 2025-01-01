
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; de : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll_drums:

	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; mark volume as requiring update
    set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

	; select rhythm trigger register
	ld a, 0xe
	out (OPLL_REG_PORT), a

	; sending note-off
	ld hl, opll_drum_note_ons
	ld a, (opll_drum_trigger)
	ld c, a
	cpl
	and a, (hl)
	out (OPLL_DATA_PORT), a

	; need to delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	; send note-on
	or a, c
	out (OPLL_DATA_PORT), a

	; store new opll_drum_note_ons status
	ld (hl), a

	; restore original hl
	ex de, hl

    ; move to midi number
    inc hl

    ; check if we should be in rhythm or fixed mode
    ld a, (opll_drum_fixed_pitch_mode)
    or a, a
    jr nz, mfmdno_fixed_pitch

        ; set pitch register for this drum	
        ; set register for low 8 bits of f number
        ld a, (opll_drum_pitch_register)
        out (OPLL_REG_PORT), a
        
        ; update midi number field
        ld a, (hl)
        ld (ix + channel.midi_note), a
        
        ; get pitch data
		add a, a
		add a, <fm_tone_lookup
		ld e, a
		adc a, >fm_tone_lookup
		sub a, e
		ld d, a

        ; write low 8 bits of f number
        ld a, (de)
        out (OPLL_DATA_PORT), a
        
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl

        ; set register for 9th bit of f number and octave
        ld a, (opll_drum_pitch_register)
        add a, 0x10
        out (OPLL_REG_PORT), a
        
        ; output 9th bit of f number and octave
        inc de
        ld a, (de)
        out (OPLL_DATA_PORT), a

    mfmdno_fixed_pitch:

	inc hl
	jp mpnl_command_done

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll_drums:

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	; select rhythm control register
	ld a, 0x0e
	out (OPLL_REG_PORT), a

	; clear trigger bit for this drum
	ld hl, opll_drum_note_ons
	ld a, (opll_drum_trigger)
	cpl
	and a, (hl)
	ld (hl), a
	
	; send note-off
	out (OPLL_DATA_PORT), a

	; restore original hl
	ex de, hl
	
	inc hl
	jp mpnl_command_done
