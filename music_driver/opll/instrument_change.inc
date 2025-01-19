
; banjo sound driver
; Joe Kennedy 2024

mpnl_opll_instrument_change:
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

	; preserve hl
	ex de, hl

	; don't do anything if the instrument number is the same
	cp a, (ix + channel.instrument_num)
	jr z, mpnl_opll_instrument_change_done
		
		; update the instrument
		call music_instrument_change

        ; coming out of music_instrument_change, hl should be pointing at the instrument data
        ; at the start of the fm information

        ; store the patch number shifted left by 4
		ld a, (hl)
		ld (ix + channel.patch), a
		
		; if it's patch 0, update the fm registers with the custom patch data
		; otherwise we're done
		or a, a
		jr nz, mpnl_opll_instrument_change_done
		
			; get address of patch data into hl
			inc hl
			ld a, (hl)
			inc hl
			ld h, (hl)
			ld l, a

			; start register number
			ld c, 0x00
			
			music_instrument_change_opll_patch:
			
				ld a, c
				out (OPLL_REG_PORT), a
				
				ld a, (hl)
				out (OPLL_DATA_PORT), a
				
				call banjo_opll_write_delay
				
				; next register and next patch data
				inc hl
				inc c
				
				; reached the last register?
				ld a, 0x08
				cp a, c
				jr nz, music_instrument_change_opll_patch

	mpnl_opll_instrument_change_done:

	; restore hl
	ex de, hl

	jp mpnl_command_done


