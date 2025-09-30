
; banjo sound driver
; Joe Kennedy 2024

; b  : current channel number
; de : instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel pointer
; iy : pointer to music state
mpnl_note_on_ay: 

	; standard note-on stuff
	ld de, #ay_tone_lookup
	call music_note_on

	jp mpnl_command_done

; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_ay: 

	; standard note-off stuff
	call music_note_off

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr nz, mnoff_sn_dont_write_chip

		; get AY_REG_WRITE
		ld c, channel.port(ix)

		; update volume level on chip
		; select volume register
		ld a, channel.subchannel(ix)
		add a, #8
		out (c), a

		; move to to AY_DATA_WRITE
		inc c

		; write 0 to register
		; setting note volume to 0 and switching off envelope
		ld a, #0
		out (c), a

	mnoff_sn_dont_write_chip: 
	
	jp mpnl_command_done_one_byte_command
