
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opn:

	; send note off if we're not playing legato
	; check for legato
	bit CHAN_FLAG_BIT_LEGATO, c
	jr nz, mnon_opn_legato_done

		; set note-off flag
		set CHAN_FLAG_BIT_NOTE_OFF, c
	
	mnon_opn_legato_done:

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
music_note_off_opn:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c

	; send note-off
	ld h, 0x28
	ld a, (ix + channel.subchannel)
	and a, 0xf
	ld l, a
	call banjo_opn_write

	jp music_note_off_opn_done
	