
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll:

	; send note off if there's a note currently playing and we're not playing legato
	; and the channel isn't muted
	bit CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)
	jr z, mnon_opll_legato_done

		; check for legato
		bit CHAN_EVENT_BIT_LEGATO, (ix + channel.events)
		jr nz, mnon_opll_legato_done
	
			; send note off
			ld a, (ix + channel.subchannel)
			add a, 0x20
			out (OPLL_REG_PORT), a

			ld a, (ix + channel.gp2)
			and a, 0xf
			out (OPLL_DATA_PORT), a

			call banjo_opll_write_delay
		
	mnon_opll_legato_done:

	; standard note-on stuff
	ld a, (de)
	call music_note_on

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_opll_dont_write_chip

		; select octave + key on register
		ld a, (ix + channel.subchannel)
		add a, 0x20
		out (OPLL_REG_PORT), a
		
		; write current octave + key off
		ld a, (ix + channel.gp2)
		and a, 0xf
		out (OPLL_DATA_PORT), a
		
		call banjo_opll_write_delay

	mnoff_opll_dont_write_chip:

	jp mpnl_command_done_one_byte_command
	