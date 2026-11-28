
; banjo sound driver
; Joe Kennedy 2024

; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : channel pointer
; iy : pointer to music state
mpnl_note_on_ay:

    .ifdef BANJO_3_57MHZ

		; sub 9 from the note number as the 3.57mhz pitch table starts at the note A
		sub a, 9
		jr nc, mpl_no_ay_nc

			xor a, a

		mpl_no_ay_nc:

    .endif

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : events and flags (preserve these!)
; ix : pointer to current channel
; iy : pointer to music state
music_note_off_ay:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c

	; preserve c
	ld l, c

	ld c, (ix + channel.port)

	; update volume level on chip
	; select volume register
	ld a, (ix + channel.subchannel)
	add a, 8
	out (c), a

	; write 0 to register
	; setting note volume to 0 and switching off envelope
	xor a, a
	inc c
	out (c), a

	; restore c
	ld c, l

	jp music_note_off_ay_done
