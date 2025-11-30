
; banjo sound driver
; Joe Kennedy 2024

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : channel pointer
; iy : pointer to music state
mpnl_note_on_ay:

    .ifdef BANJO_3_57MHZ
		; add 3 to the note number as the 3.57mhz pitch table starts at the note A
		add a, 3
    .endif

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_ay:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_sn_dont_write_chip

		push bc

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

		pop bc

	mnoff_sn_dont_write_chip:
	
	jp mpnl_command_done_one_byte_command