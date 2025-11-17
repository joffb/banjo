
; banjo sound driver
; Joe Kennedy 2024

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opn_instrument_change:

	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

		; don't do anything if the instrument number is the same
		ld a, (de)
		cp a, (ix + channel.instrument_num)
		jr z, mpnl_opn_instrument_change_done
			
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

			; OPNA channels 3-5?
			ld b, (ix + channel.subchannel)
			bit 2, b
			jr z, mpnl_opn_ic_first_ports

				; use second set of OPNA ports
				ld a, (banjo_opna_port)
				ld c, a

				ld a, 0x3
				and a, b
				
				jr mnpl_opn_ic_port_selected

			mpnl_opn_ic_first_ports:

				; use second set of OPNA ports
				ld a, (banjo_opn_port)
				ld c, a

				ld a, 0x3
				and a, b

			mnpl_opn_ic_port_selected:

			; use first set of OPN ports
			; first instrument registers at 0x30
			add a, 0x30

			; write out values
			; 7 sets of registers with four operators
			; multiply by 2 as we're doing outi and djnz
			ld b, 7 * 4 * 2
			monic_loop:

				; set reg
				out (c), a

				; get value and write it out on data port
				inc c
				outi

				call banjo_opn_write_delay_long

				; move back to reg port
				dec c

				; move on to next register
				add a, 4

				djnz monic_loop

			; feedback and algorithm/connection register

			; register in `a` should be at 0xa0 - 0xa2 at this point
			; move along to 0xb0

			; set reg
			add a, 0x10
			out (c), a
			inc c

			; get value and write it out
			outi
			dec c
			
			call banjo_opn_write_delay_short

			; pms/ams/lr

			; set reg for channel
			add a, 0x04
			out (c), a
			inc c

			; get reg value and write it out
			outi
			dec c
			
			call banjo_opn_write_delay_short

			; store pointer to rest of patch data
			; used for volume changes
			ld (ix + channel.gp2), l
			ld (ix + channel.gp3), h

			; restore bc
			pop bc

	mpnl_opn_instrument_change_done:

	jp mpnl_command_done


