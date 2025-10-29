
; banjo sound driver
; Joe Kennedy 2024

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : channel pointer
; iy : pointer to music state
mpnl_note_on_ay:

	; standard note-on stuff
	ld hl, ay_tone_lookup
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

		; update volume level on chip
		; select volume register
		ld a, (ix + channel.subchannel)
		add a, 8
		out (AY_REG_WRITE), a

		; write 0 to register
		; setting note volume to 0 and switching off envelope
		xor a, a
		out (AY_DATA_WRITE), a

	mnoff_sn_dont_write_chip:
	
	jp mpnl_command_done_one_byte_command