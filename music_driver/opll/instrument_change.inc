
; banjo sound driver
; Joe Kennedy 2024

banjo_instrument_change_opll:

	; get patch data pointer into hl
	ex de, hl

	; store the patch number shifted left by 4
	ld a, (hl)
	ld (ix + channel.gp1), a
	inc hl

	; don't write fm patch data unless this is the user patch 0
	or a, a
	jp nz, banjo_instrument_change_done

		; preserve bc
		push bc
		
		; 8 registers, multiplied by 2
		; as b will be decremented by both outi and djnz
		ld b, 16

		; destination for outi
		ld c, OPLL_DATA_PORT
		
		; start at register 0
		xor a, a
		
		music_instrument_change_opll_patch:
		
			; set opll register
			out (OPLL_REG_PORT), a
			inc a
			
			; write new value from (hl)
			outi
			
			call banjo_opll_write_delay
			
			; reached the last register?
			djnz music_instrument_change_opll_patch
			
		; restore bc
		pop bc

		jp banjo_instrument_change_done


