
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll:

	; no note-off if playing legato
	bit CHAN_FLAG_BIT_LEGATO, c
	jr nz, mnon_opll_legato_done

		; set note-off flag
		set CHAN_FLAG_BIT_NOTE_OFF, c
			
	mnon_opll_legato_done:

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
music_note_off_opll:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c

	; select octave + key on register
	ld a, (ix + channel.subchannel)
	add a, 0x20
	out (OPLL_REG_PORT), a
	
	; write current octave + key off
	ld a, (ix + channel.gp2)
	and a, 0xf
	out (OPLL_DATA_PORT), a
	
	jp music_note_off_opll_done
