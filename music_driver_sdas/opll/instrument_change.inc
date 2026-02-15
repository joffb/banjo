
; banjo sound driver
; Joe Kennedy 2024

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opll_instrument_change: 

	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

	; don't do anything if the instrument number is the same
	ld a, (de)
	cp a, channel.instrument_num(ix)
	jr z, mpnl_opll_instrument_change_done
		
		; update the instrument
		call music_instrument_change

        ; coming out of music_instrument_change, hl should be pointing at the instrument data
        ; at the start of the fm information

        ; store the patch number shifted left by 4
		ld a, (hl)
		ld channel.gp1(ix), a
		
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

			; preserve bc
			push bc
			
			; 8 registers, multiplied by 2
			; as b will be decremented by both outi and djnz
			ld b, #16

			; destination for outi
			ld c, #OPLL_DATA_PORT
			
			; start at register 0
			xor a, a
			
			music_instrument_change_opll_patch: 
			
				; set opll register
				out (#OPLL_REG_PORT), a
				inc a
				
				; write new value from (hl)
				outi 
				
				call _banjo_opll_write_delay
				
				; reached the last register?
				djnz music_instrument_change_opll_patch
				
			; restore bc
			pop bc

	mpnl_opll_instrument_change_done: 

	jp mpnl_command_done


