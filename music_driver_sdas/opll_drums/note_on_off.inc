
; banjo sound driver
; Joe Kennedy 2023

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll_drums: 

	; preserve bc
	push bc

	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

    ; mark volume as requiring update
    set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

	; select rhythm trigger register
	ld a, #0xe
	out (#OPLL_REG_PORT), a

	; sending note-off
	ld hl, #opll_drum_note_ons

	; get drum trigger bit
	; and use it to clear the bit for this drum
	ld a, channel.gp2(ix)
	ld c, a
	cpl 
	and a, (hl)
	out (#OPLL_DATA_PORT), a

	call _banjo_opll_write_delay

	; send note-on
	or a, c
	out (#OPLL_DATA_PORT), a

	; store new opll_drum_note_ons status
	ld (hl), a

    ; check if we should be in rhythm or fixed mode
    ld a, (opll_drum_fixed_pitch_mode)
    or a, a
    jr nz, mfmdno_fixed_pitch

        ; update midi number field
        ld a, (de)
		and a, #0x7f
        ld channel.midi_note(ix), a

        ; update freq
        ld channel.freq+1(ix), a
		xor a, a
		ld channel.freq(ix), a

		; mark pitch as requiring update
		set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

    mfmdno_fixed_pitch: 
	
	; restore bc
	pop bc

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll_drums: 

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; select rhythm control register
	ld a, #0x0e
	out (#OPLL_REG_PORT), a

	; sending note-off
	ld hl, #opll_drum_note_ons

	; get drum trigger bit
	; and use it to clear the bit for this drum
	ld a, channel.gp2(ix)
	cpl 
	and a, (hl)
	ld (hl), a
	
	; send note-off
	out (#OPLL_DATA_PORT), a

	jp mpnl_command_done_one_byte_command
