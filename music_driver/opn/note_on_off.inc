
; banjo sound driver
; Joe Kennedy 2023

; a : midi note
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opn:

	ld a, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, a
	jr nz, mnon_opn_dont_write_chip

		; send note off if there's a note currently playing and we're not playing legato
		; and the channel isn't muted
		bit CHAN_FLAG_BIT_NOTE_ON, a
		jr z, mnon_opn_legato_done

			; check for legato
			bit CHAN_EVENT_BIT_LEGATO, (ix + channel.events)
			jr nz, mnon_opn_legato_done

				; send note-off
				ld h, 0x28
				ld l, (ix + channel.subchannel)
				call banjo_opn_write
			
		mnon_opn_legato_done:

		; send note-on
		ld h, 0x28
		ld a, (ix + channel.subchannel)
		or a, 0xf0
		ld l, a
		call banjo_opn_write

	mnon_opn_dont_write_chip:

	; standard note-on stuff
	ld a, (de)
	call music_note_on

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opn:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_opn_dont_write_chip

		; send note-off
		ld h, 0x28
		ld l, (ix + channel.subchannel)
		call banjo_opn_write

	mnoff_opn_dont_write_chip:

	jp mpnl_command_done_one_byte_command
	