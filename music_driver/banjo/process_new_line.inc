
; banjo sound driver
; Joe Kennedy 2023

; bc: jump table to handle commands (e.g. music_command_jump_table_sn)
; ix: channels
; iy: state
music_process_new_line:

	ld b, (ix + channel.events)
	ld c, (ix + channel.flags)

    ; note delay?
    bit CHAN_FLAG_BIT_TIC_WAIT, c
    jr z, mpnl_no_tic_wait

        ; decrement tic wait and return if not zero
        dec (ix + channel.wait)
        ret nz

			; tic wait is now zero
			; reset tic wait flag and continue
			res CHAN_FLAG_BIT_TIC_WAIT, c
			jr music_process_new_line_continue

    mpnl_no_tic_wait:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    ret z

		; is there a line wait?
		ld a, (ix + channel.wait)
		or a, a
		jr z, music_process_new_line_continue

			; decrease line wait and return if not zero
			dec (ix + channel.wait)
			ret

    music_process_new_line_continue:

	; get current pattern pointer for this channel
	ld e, (ix + channel.pattern_ptr)
	ld d, (ix + channel.pattern_ptr + 1)

	mpnl_loop:
	
		; get command
		ld a, (de)
		
		; if it's < 32 it's a jump table command
		cp a, 32
		jr c, mpnl_use_jump_table
		
		; is this a note on? (bit 7 set)
		bit 7, a
		jp z, mpnl_not_note_on

			; preserve jump table address
			push hl

			; get jump address
			ld a, (hl)
			inc hl
			ld h, (hl)
			ld l, a

			; get note-on value
			ld a, (de)
			and a, 0x7f

			; jump to jump table address
			jp (hl)
			
		; is the end of this line for this channel? (bit 6 set)
		; if not, use jump table
		mpnl_not_note_on:
		bit 6, a
		jp z, mpnl_not_eol

			; end of this line reached
			; get line_wait amount and set it in channel
			and a, 0x3f
			ld (ix + channel.wait), a

			; store the pointer to the next command
			inc de
			ld (ix + channel.pattern_ptr), e
			ld (ix + channel.pattern_ptr + 1), d

			ret
			
		; must be a volume change (bit 5 set)
		mpnl_not_eol:

			; offset to volume change command
			ld a, 3
			dec de

		; if not, process the command
		mpnl_use_jump_table:
			
			; move pattern pointer on
			inc de

			; preserve jump table address
			push hl

			; offset into jump table
			add a, a
			add a, l
			ld l, a
			adc a, h
			sub a, l
			ld h, a

			; get jump address
			ld a, (hl)
			inc hl
			ld h, (hl)
			ld l, a

			; jump to jump table address
			jp (hl)

		mpnl_command_done:
		
			inc de

		mpnl_command_done_one_byte_command:

			; restore jump table address
			pop hl
			
			jr mpnl_loop
