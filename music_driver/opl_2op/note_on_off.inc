
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opl_2op:

	; send note off if we're not playing legato
	; check for legato
	bit CHAN_FLAG_BIT_LEGATO, c
	jr nz, mnon_opl_2op_legato_done

		; set note-off flag
		set CHAN_FLAG_BIT_NOTE_OFF, c
	
	mnon_opl_2op_legato_done:

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : events and flags (preserve these!)
; ix : pointer to current channel
; iy : pointer to music state
music_note_off_opl2op:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c

	; select octave + key on register
	ld a, (ix + channel.subchannel)
	add a, 0xb0
	out (OPL_REG_PORT), a
	
	; write current octave + key off
	ld a, (ix + channel.gp1)
	and a, 0x1f
	out (OPL_DATA_PORT), a
	
	call banjo_opl_write_delay

	jp music_note_off_opl2op_done
	