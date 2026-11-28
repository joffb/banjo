
; banjo sound driver
; Joe Kennedy 2023

; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll_drums:

	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, c

    ; mark volume as requiring update
    set CHAN_EVENT_BIT_VOLUME_CHANGE, b

	; pointer to note-on byte
	ld hl, opll_drum_note_ons

	; set drum trigger bit in opll_drum_note_ons
	; and use it to set note-on bit to signal we need a note-on
	ld a, (ix + channel.gp2)
	or a, (hl)
	ld (hl), a

	; set note-off bit in opll_drum_note_offs
	inc hl
	ld a, (ix + channel.gp2)
	or a, (hl)
	ld (hl), a

    ; check if we should be in rhythm or fixed mode
    ld a, (opll_drum_fixed_pitch_mode)
    or a, a
    jr nz, mfmdno_fixed_pitch

        ; get note
        ld a, (de)
		and a, 0x7f

        ; update freq
        ld (ix + channel.freq + 1), a
		xor a, a
		ld (ix + channel.freq), a

		; mark pitch as requiring update
		set CHAN_EVENT_BIT_PITCH_CHANGED, b

    mfmdno_fixed_pitch:

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll_drums:

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, c

	; set note-off bit in opll_drum_note_offs
	ld hl, opll_drum_note_offs
	ld a, (ix + channel.gp2)
	or a, (hl)
	ld (hl), a

	jp mpnl_command_done_one_byte_command
