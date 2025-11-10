
; banjo sound driver
; Joe Kennedy 2024

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opl_2op_instrument_change:

	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

	; don't do anything if the instrument number is the same
	ld a, (de)
	cp a, (ix + channel.instrument_num)
	jr z, mpnl_opl_2op_instrument_change_done
		
		; update the instrument
		call music_instrument_change

        ; coming out of music_instrument_change, hl should be pointing at the instrument data
        ; at the start of the fm information

		; get address of patch data into hl
		inc hl
		ld a, (hl)
		inc hl
		ld h, (hl)
		ld l, a

		; preserve bc
		push bc
		
		; get index of first operator for this channel, keep in c
		ld a, (ix + channel.subchannel)
		call banjo_opl_2op_get_first_slot_index
		ld c, a

		; write out values for operators
		ld b, 4
		mo2ic_loop:

			; set reg for first operator
			; move on by 0x20 for each set of slot operators
			ld a, c
			add a, 0x20
			out (OPL_REG_PORT), a

			; keep reg in c
			ld c, a

			; get reg value and write it out
			ld a, (hl)
			out (OPL_DATA_PORT), a
			inc hl

			call banjo_opl_write_delay

			; move on to second operator
			ld a, c
			add a, 3
			out (OPL_REG_PORT), a

			; get reg value and write it out
			ld a, (hl)
			out (OPL_DATA_PORT), a
			inc hl

			call banjo_opl_write_delay

			djnz mo2ic_loop

		; feedback and algorithm/connection register

		; set reg for channel
		ld a, 0xc0
		add a, (ix + channel.subchannel)
		out (OPL_REG_PORT), a

		; get reg value and write it out
		ld a, (hl)
		out (OPL_DATA_PORT), a
		inc hl
		
		call banjo_opl_write_delay

		; waveform registers

		; set reg for first operator
		ld a, c
		or a, 0xe0
		out (OPL_REG_PORT), a

		; keep reg in c
		ld c, a

		; get reg value and write it out
		ld a, (hl)
		out (OPL_DATA_PORT), a
		inc hl

		call banjo_opl_write_delay

		; move on to second operator
		ld a, c
		add a, 3
		out (OPL_REG_PORT), a

		; get reg value and write it out
		ld a, (hl)
		out (OPL_DATA_PORT), a
		inc hl

		; store pointer to rest of patch data
		; used for volume changes
		ld (ix + channel.gp2), l
		ld (ix + channel.gp3), h

		; restore bc
		pop bc

	mpnl_opl_2op_instrument_change_done:

	jp mpnl_command_done


