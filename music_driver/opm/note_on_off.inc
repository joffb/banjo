
; banjo sound driver
; Joe Kennedy 2025

; a : midi note
; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opm:

	; send note off if we're not playing legato
	; check for legato
	bit CHAN_FLAG_BIT_LEGATO, c
	jr nz, mnon_opm_legato_done

		; set note-off flag
		set CHAN_FLAG_BIT_NOTE_OFF, c
	
	mnon_opm_legato_done:

    ; send note on
    ld h, 0x8
    ld l, (ix + channel.subchannel)
    call banjo_opm_write

	; standard note-on stuff
	ld a, (de)
	call music_note_on

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
music_note_off_opm:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c
	
	; send note off
	ld h, 0x8
	ld a, (ix + channel.subchannel)
	and a, 0x7
	ld l, a
	call banjo_opm_write

	jp music_note_off_opm_done
	