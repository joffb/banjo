
; banjo sound driver
; Joe Kennedy 2023

; b: channel number
; bc: jump table to handle commands (e.g. music_command_jump_table_sn)
; ix: channels
; iy: state
music_process_new_line: 
	
	; don't do anything if line_wait > 0
	ld a, channel.line_wait(ix)
	or a, a
	jr z, mpnl_no_line_wait

		; update line_wait value and return
		dec channel.line_wait(ix)
		ret 

	mpnl_no_line_wait: 

	; get current pattern pointer for this channel
	ld e, channel.pattern_ptr(ix)
	ld d, channel.pattern_ptr+1(ix)
	
	mpnl_loop: 
	
		; get command
		ld a, (de)
		
		; if it's < 32 it's a jump table command
		cp a, #32
		jr c, mpnl_use_jump_table
		
		; is this a note on? (bit 7 set)
		bit 7, a
		jp z, mpnl_not_note_on
			
			; offset jump table address to note-on command
			ld l, c
			ld h, b

			; jump to jump table address
			jp (hl)
			
		; is the end of this line for this channel? (bit 6 set)
		; if not, use jump table
		mpnl_not_note_on: 
		bit 6, a
		jp z, mpnl_not_eol

			; end of this line reached
			; get line_wait amount and set it in channel
			and a, #0x3f
			ld channel.line_wait(ix), a

			; store the pointer to the next command and return
			inc de
			ld channel.pattern_ptr(ix), e
			ld channel.pattern_ptr+1(ix), d

			ret 
			
		; must be a volume change (bit 5 set)
		mpnl_not_eol: 
			
			; offset jump table address to volume change command
			ld hl, #9
			add hl, bc
			
			; jump to jump table address
			jp (hl)

		; if not, process the command
		mpnl_use_jump_table: 

			; address in table is command num * 3
			; use it to offset into the jump table
			ld l, a
			add a, a
			add a, l
			ld l, a
			ld h, #0
			add hl, bc

			; move pattern pointer on and read
			; the next byte into a in preparation
			inc de
			ld a, (de)

			; jump to jump table address
			jp (hl)

		mpnl_command_done: 
		
			inc de

		mpnl_command_done_one_byte_command: 
			
			jr mpnl_loop
