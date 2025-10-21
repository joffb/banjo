
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll: 

	; standard note-on stuff
	ld hl, #fm_tone_lookup
	call music_note_on

	; send note off if there's a note currently playing and we're not playing legato
	; and the channel isn't muted
	ld a, channel.flags(ix)
	and a, #CHAN_FLAG_NOTE_ON|CHAN_FLAG_MUTED
	cp a, #CHAN_FLAG_NOTE_ON
	jr nz, mnon_opll_legato_done

		; check for legato
		bit CHAN_EVENT_BIT_LEGATO, channel.events(ix)
		jr nz, mnon_opll_legato_done
	
		; send note off
		ld a, channel.subchannel(ix)
        add a, #0x20
		out (#OPLL_REG_PORT), a

		ld a, channel.freq+1(ix)
		and a, #0xf
		out (#OPLL_DATA_PORT), a

		call _banjo_opll_write_delay
		
	mnon_opll_legato_done: 

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll: 

	; standard note-off stuff
	call music_note_off

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr nz, mnoff_opll_dont_write_chip

		; select octave + key on register
		ld a, channel.subchannel(ix)
		add a, #0x20
		out (#OPLL_REG_PORT), a
		
		; write current octave + key off
		ld a, channel.freq+1(ix)
		and a, #0xf
		out (#OPLL_DATA_PORT), a
		
		call _banjo_opll_write_delay

	mnoff_opll_dont_write_chip: 

	jp mpnl_command_done_one_byte_command
	
