
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opl_2op:

	; standard note-on stuff
	ld hl, opl_2op_tone_lookup
	call music_note_on

	; send note off if there's a note currently playing and we're not playing legato
	; and the channel isn't muted
	ld a, (ix + channel.flags) 
	and a, CHAN_FLAG_NOTE_ON | CHAN_FLAG_MUTED
	cp a, CHAN_FLAG_NOTE_ON
	jr nz, mnon_opl_2op_legato_done

		; check for legato
		bit CHAN_EVENT_BIT_LEGATO, (ix + channel.events)
		jr nz, mnon_opl_2op_legato_done
	
			; send note off
			ld a, (ix + channel.subchannel)
			add a, 0xb0
			out (OPL_REG_PORT), a

			ld a, (ix + channel.gp1)
			and a, 0x1f
			out (OPL_DATA_PORT), a

			call banjo_opl_write_delay
		
	mnon_opl_2op_legato_done:

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opl_2op:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_opl_2op_dont_write_chip

		; select octave + key on register
		ld a, (ix + channel.subchannel)
		add a, 0xb0
		out (OPL_REG_PORT), a
		
		; write current octave + key off
		ld a, (ix + channel.gp1)
		and a, 0x1f
		out (OPL_DATA_PORT), a
		
		call banjo_opl_write_delay

	mnoff_opl_2op_dont_write_chip:

	jp mpnl_command_done_one_byte_command
	