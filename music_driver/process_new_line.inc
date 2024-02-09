
; banjo sound driver
; Joe Kennedy 2023

; ix: channels
; iy: state
music_process_new_line:

	; number of channels to process
	ld b, (iy + music_state.channel_count)
	
	music_process_new_line_loop:
		
		; don't do anything if line_wait > 0
		ld a, (ix + channel.line_wait)
		or a, a
		jr z, mpnll_no_line_wait

			; update line_wait value
			dec a
			ld (ix + channel.line_wait), a

			; move to next channel or finish
			ld de, _sizeof_channel
			add ix, de
		
			djnz music_process_new_line_loop
			ret

		mpnll_no_line_wait:

		ld l, (ix + channel.pattern_ptr)
		ld h, (ix + channel.pattern_ptr + 1)
		
		music_process_new_line_subloop:
		
			; get command
			ld a, (hl)
			
            ; is the end of this line for this channel?
			cp a, END_PATTERN_LINE
			jp z, mpnll_line_end

            mpnl_use_jump_table:

                ; preserve hl
                ; we want to use hl's ability to ld into other registers than a, e.g. "ld h, (hl)"
                ex de, hl

				; double command num
				add a, a

                ; use it to offset into the jump table
                ld hl, music_process_jump_table 
                add a, l
                ld l, a
				adc a, h
				sub a, l
				ld h, a

                ; get address into hl
                ld c, (hl)
                inc hl
                ld h, (hl)
                ld l, c

                ; push address onto stack
                push hl

                ; restore original hl
                ex de, hl

                ; jump to address we just pushed onto stack
                ret
                            
			mpnl_note_on:
			
				; check if the channel is muted
				ld a, (ix + channel.flags)
				and a, CHAN_FLAG_MUTED
				jr z, mpnl_note_on_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					inc hl
					jr music_process_new_line_subloop
				
				mpnl_note_on_not_muted:
			
				; reset volume macro position and pointer if we have one
				ld a, (ix + channel.volume_macro_len)
				or a, a 
				jr z, music_process_new_line_no_volume_macro
					
                    ; subtract current macro position from the 
                    ; volume macro pointer address to reset it
                    ; to the start
                    ld a, (ix + channel.volume_macro_ptr)
					sub a, (ix + channel.volume_macro_pos)
                    ld (ix + channel.volume_macro_ptr), a

                    ; reset pos with c = 0 so we can use 
                    ; c = 0 again with sbc
                    ld c, 0
                    ld (ix + channel.volume_macro_pos), c

                    ; update the upper byte of the macro's address
					ld a, (ix + channel.volume_macro_ptr + 1)
					sbc a, c
					ld (ix + channel.volume_macro_ptr + 1), a
			
				music_process_new_line_no_volume_macro:
				
				; mark pitch as requiring update
				ld a, (ix + channel.flags)
				or a, CHAN_FLAG_PITCH_CHANGED
				ld (ix + channel.flags), a
				
				; different note-on procedures per-channel type
				; they should all do
				;	jp music_process_new_line_subloop
				; when they're done
				ld a, (ix + channel.type)
				
				; sn note on				
				cp a, CHAN_SN76489
				jp z, music_sn_note_on
				
				; fm note on
				cp a, CHAN_OPLL
				jp z, music_fm_note_on

				; fm drum note on
				cp a, CHAN_OPLL_DRUMS
				jp z, music_fm_drums_note_on

			mpnl_note_off:
			
				; check if the channel is muted
				ld a, (ix + channel.flags)
				and a, CHAN_FLAG_MUTED
				jr z, mpnl_note_off_not_muted
				
					; don't do anything if it is
					inc hl
					jr music_process_new_line_subloop
				
				mpnl_note_off_not_muted:
				
				; different note-off procedures per-channel type
				; they should all do
				;	jp music_process_new_line_subloop
				; when they're done
				ld a, (ix + channel.type)
				
				cp a, CHAN_SN76489
				jp z, music_sn_note_off
				
				cp a, CHAN_OPLL
				jp z, music_fm_note_off
				
				cp a, CHAN_OPLL_DRUMS
				jp z, music_fm_drums_note_off

			mpnl_instrument_change:
						
				; check if the channel is muted
				ld a, (ix + channel.flags)
				and a, CHAN_FLAG_MUTED
				jr z, mpnl_instrument_change_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
				
				mpnl_instrument_change_not_muted:
				jp music_instrument_change
			
            mpnl_fm_drum:

				; check if the channel is muted
				ld a, (ix + channel.flags)
				and a, CHAN_FLAG_MUTED
				jr z, mpnl_fm_drum_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
				
				mpnl_fm_drum_not_muted:
				jp music_fm_drum_change

			mpnl_noise_mode:
			
				; check if the channel is muted
				ld a, (ix + channel.flags)
				and a, CHAN_FLAG_MUTED
				jr z, mpnl_noise_mode_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
			
				mpnl_noise_mode_not_muted:
				
				ld c, (ix + channel.port)

				; update noise mode 
				inc hl
				ld a, (hl)
				out (c), a
				
				; store for later
				ld (iy + music_state.noise_mode), a
				
				inc hl
				jp music_process_new_line_subloop
				
			mpnl_pitch_slide_up:
			
				; get slide amount and store it for later
				inc hl
				ld a, (hl)
				ld (ix + channel.slide_amount), a
				
				; set slide_type = 1
				ld (ix + channel.slide_type), SLIDE_TYPE_UP
				
				inc hl				
				jp music_process_new_line_subloop
				
			mpnl_pitch_slide_down:
			
				; get slide amount and store it for later
				inc hl
				ld a, (hl)
				ld (ix + channel.slide_amount), a
				
				; set slide_type = 2
				ld (ix + channel.slide_type), SLIDE_TYPE_DOWN
				
				inc hl				
				jp music_process_new_line_subloop

			mpnl_arpeggio:

				; load arpeggio offsets
				inc hl
				ld a, (hl)
				ld (ix + channel.arpeggio), a

				inc hl
				ld a, (hl)
				ld (ix + channel.arpeggio + 1), a

				; set arpeggio position to 0
				ld (ix + channel.arpeggio_pos), 0

				inc hl
				jp music_process_new_line_subloop
				
			mpnl_portamento:

				; load portamento amount
				inc hl
				ld a, (hl)
				ld (ix + channel.slide_amount), a

				; set slide_type = 3
				ld (ix + channel.slide_type), SLIDE_TYPE_PORTA

				inc hl
				jp music_process_new_line_subloop

			mpnl_line_wait:

				; get line_wait amount
				inc hl
				ld a, (hl)
				ld (ix + channel.line_wait), a
				jr mpnll_line_end

			mpnl_instruction_check_done:
			
				inc hl
				jp music_process_new_line_subloop

			mpnll_line_end:
				
				; end of this line reached
				; store the pointer to the next command and move to the next channel
				inc hl
				ld (ix + channel.pattern_ptr), l
				ld (ix + channel.pattern_ptr + 1), h	

                ld de, _sizeof_channel
                add ix, de
            
				dec b
                jp nz, music_process_new_line_loop

                ; last channel processed, we're done
                ret