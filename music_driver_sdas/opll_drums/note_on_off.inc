
; banjo sound driver
; Joe Kennedy 2023

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll_drums: 

	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

    ; mark volume as requiring update
    set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

	; pointer to note-on byte
	ld hl, #opll_drum_note_ons

	; set drum trigger bit in opll_drum_note_ons
	; and use it to set note-on bit to signal we need a note-on
	ld a, channel.gp2(ix)
	or a, (hl)
	ld (hl), a

	; set note-off bit in opll_drum_note_offs
	inc hl
	ld a, channel.gp2(ix)
	or a, (hl)
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

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll_drums: 

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; set note-off bit in opll_drum_note_offs
	ld hl, #opll_drum_note_offs
	ld a, channel.gp2(ix)
	or a, (hl)
	ld (hl), a

	jp mpnl_command_done_one_byte_command
