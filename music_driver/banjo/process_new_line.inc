
; banjo sound driver
; Joe Kennedy 2023

; b: channel number
; de: jump table to handle commands (e.g. music_command_jump_table_sn)
; ix: channels
; iy: state
music_process_new_line:
	
	; don't do anything if line_wait > 0
	ld a, (ix + channel.line_wait)
	or a, a
	jr z, mpnl_no_line_wait

		; update line_wait value and return
		dec a
		ld (ix + channel.line_wait), a
		ret

	mpnl_no_line_wait:

	; get current pattern pointer for this channel
	ld l, (ix + channel.pattern_ptr)
	ld h, (ix + channel.pattern_ptr + 1)
	
	mpnl_loop:
	
		; get command
		ld a, (hl)
		
		; if it's < 32 it's a jump table command
		cp a, 32
		jr c, mpnl_use_jump_table
		
		; is this a note on? (bit 7 set)
		bit 7, a
		jp z, mpnl_not_note_on
		
			; preserve commands jump table address (currently in de)
			push de

			; preserve pattern pointer (currently in hl) in de
			; get jump table address in hl
			ex de, hl
			
			; get address from table into bc
			ld c, (hl)
			inc hl
			ld b, (hl)
			
			; restore pattern pointer to hl
			; jump address is in de
			ex de, hl

			; jump to jump table address
			push bc
			ret
			
		; is the end of this line for this channel? (bit 6 set)
		; if not, use jump table
		mpnl_not_note_on:
		bit 6, a
		jp z, mpnl_not_eol

			; end of this line reached
			; get line_wait amount and set it in channel
			and a, 0x3f
			ld (ix + channel.line_wait), a

			; store the pointer to the next command and return
			inc hl
			ld (ix + channel.pattern_ptr), l
			ld (ix + channel.pattern_ptr + 1), h

			ret
			
		; must be a volume change (bit 5 set)
		mpnl_not_eol:
		
			; preserve commands jump table address (currently in de)
			push de

			; preserve pattern pointer (currently in hl) in de
			; get jump table address in hl
			ex de, hl
			
			; move jump table address to volume change command
			ld bc, 6
			add hl, bc
			
			; get address from table into bc
			ld c, (hl)
			inc hl
			ld b, (hl)
			
			; restore pattern pointer to hl
			; jump address is in de
			ex de, hl

			; jump to jump table address
			push bc
			ret

		; if not, process the command
		mpnl_use_jump_table:

			; preserve commands jump table address (currently in de)
			push de

			; preserve pattern pointer (currently in hl) in de
			; we want to use hl's ability to jump
			ex de, hl

			; address in table is command num * 2
			; use it to offset into the jump table
			; the address of which was in de and is now in hl
			add a, a
			ld c, a
			ld b, 0
			add hl, bc

			; get address from table into bc
			ld c, (hl)
			inc hl
			ld b, (hl)

			; restore pattern pointer to hl
			; jump address is in de
			ex de, hl

			; move pattern pointer on and read
			; the next byte into a in preparation
			inc hl
			ld a, (hl)

			; jump to jump table address
			push bc
			ret

		mpnl_command_done:

			inc hl

		mpnl_command_done_one_byte_command:

			; restore jump table address and continue looping
			pop de
			
			jr mpnl_loop
