
; banjo sound driver
; Joe Kennedy 2023

; ix: channels
; iy: state
music_process_new_line: 

	; number of channels to process
	ld b, music_state.channel_count(iy)
	
	music_process_new_line_loop: 
		
		; don't do anything if line_wait > 0
		ld a, channel.line_wait(ix)
		or a, a
		jr z, mpnll_no_line_wait

			; update line_wait value
			dec a
			ld channel.line_wait(ix), a

			; move to next channel or finish
			ld de, #_sizeof_channel
			add ix, de
		
			djnz music_process_new_line_loop
			ret 

		mpnll_no_line_wait: 

		ld l, channel.pattern_ptr(ix)
		ld h, channel.pattern_ptr+1(ix)
		
		music_process_new_line_subloop: 
		
			; get command
			ld a, (hl)
			
            ; is the end of this line for this channel?
			cp a, #END_PATTERN_LINE
			jp z, mpnll_line_end

            mpnl_use_jump_table: 

                ; preserve hl
                ; we want to use hl's ability to ld into other registers than a, e.g. "ld h, (hl)"
                ex de, hl

				; double command num
				add a, a

                ; use it to offset into the jump table
                ld hl, #music_process_jump_table
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
				ld a, channel.flags(ix)
				and a, #CHAN_FLAG_MUTED
				jr z, mpnl_note_on_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					inc hl
					jr music_process_new_line_subloop
				
				mpnl_note_on_not_muted: 
			
				; reset volume macro position and pointer if we have one
				ld a, channel.volume_macro_len(ix)
				or a, a
				jr z, music_process_new_line_no_volume_macro
					
                    ; subtract current macro position from the 
                    ; volume macro pointer address to reset it
                    ; to the start
                    ld a, channel.volume_macro_ptr(ix)
					sub a, channel.volume_macro_pos(ix)
                    ld channel.volume_macro_ptr(ix), a

                    ; reset pos with c = 0 so we can use 
                    ; c = 0 again with sbc
                    ld c, #0
                    ld channel.volume_macro_pos(ix), c

                    ; update the upper byte of the macro's address
					ld a, channel.volume_macro_ptr+1(ix)
					sbc a, c
					ld channel.volume_macro_ptr+1(ix), a
			
				music_process_new_line_no_volume_macro: 
				
				; mark pitch as requiring update
				ld a, channel.flags(ix)
				or a, #CHAN_FLAG_PITCH_CHANGED
				ld channel.flags(ix), a
				
				; different note-on procedures per-channel type
				; they should all do
				;	jp music_process_new_line_subloop
				; when they're done
				ld a, channel.type(ix)
				
				; sn note on				
				cp a, #CHAN_SN76489
				jp z, music_sn_note_on
				
				; fm note on
				cp a, #CHAN_OPLL
				jp z, music_fm_note_on

				; fm drum note on
				cp a, #CHAN_OPLL_DRUMS
				jp z, music_fm_drums_note_on

			mpnl_note_off: 
			
				; check if the channel is muted
				ld a, channel.flags(ix)
				and a, #CHAN_FLAG_MUTED
				jr z, mpnl_note_off_not_muted
				
					; don't do anything if it is
					inc hl
					jr music_process_new_line_subloop
				
				mpnl_note_off_not_muted: 
				
				; different note-off procedures per-channel type
				; they should all do
				;	jp music_process_new_line_subloop
				; when they're done
				ld a, channel.type(ix)
				
				cp a, #CHAN_SN76489
				jp z, music_sn_note_off
				
				cp a, #CHAN_OPLL
				jp z, music_fm_note_off
				
				cp a, #CHAN_OPLL_DRUMS
				jp z, music_fm_drums_note_off

			mpnl_instrument_change: 
						
				; check if the channel is muted
				ld a, channel.flags(ix)
				and a, #CHAN_FLAG_MUTED
				jr z, mpnl_instrument_change_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
				
				mpnl_instrument_change_not_muted: 
				jp music_instrument_change
			
            mpnl_fm_drum: 

				; check if the channel is muted
				ld a, channel.flags(ix)
				and a, #CHAN_FLAG_MUTED
				jr z, mpnl_fm_drum_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
				
				mpnl_fm_drum_not_muted: 
				jp music_fm_drum_change

			mpnl_noise_mode: 
			
				; check if the channel is muted
				ld a, channel.flags(ix)
				and a, #CHAN_FLAG_MUTED
				jr z, mpnl_noise_mode_not_muted
				
					; don't do anything if it is
					inc hl
					inc hl
					jp music_process_new_line_subloop
			
				mpnl_noise_mode_not_muted: 
				
				ld c, channel.port(ix)

				; update noise mode 
				inc hl
				ld a, (hl)
				out (c), a
				
				; store for later
				ld music_state.noise_mode(iy), a
				
				inc hl
				jp music_process_new_line_subloop
				
			mpnl_pitch_slide_up: 
			
				; get slide amount and store it for later
				inc hl
				ld a, (hl)
				ld channel.slide_amount(ix), a
				
				; set slide_type = 1
				ld channel.slide_type(ix), #SLIDE_TYPE_UP
				
				inc hl
				jp music_process_new_line_subloop
				
			mpnl_pitch_slide_down: 
			
				; get slide amount and store it for later
				inc hl
				ld a, (hl)
				ld channel.slide_amount(ix), a
				
				; set slide_type = 2
				ld channel.slide_type(ix), #SLIDE_TYPE_DOWN
				
				inc hl
				jp music_process_new_line_subloop

			mpnl_arpeggio: 

				; load arpeggio offsets
				inc hl
				ld a, (hl)
				ld channel.arpeggio(ix), a

				inc hl
				ld a, (hl)
				ld channel.arpeggio+1(ix), a

				; set arpeggio position to 0
				ld channel.arpeggio_pos(ix), #0

				inc hl
				jp music_process_new_line_subloop
				
			mpnl_portamento: 

				; load portamento amount
				inc hl
				ld a, (hl)
				ld channel.slide_amount(ix), a

				; set slide_type = 3
				ld channel.slide_type(ix), #SLIDE_TYPE_PORTA

				inc hl
				jp music_process_new_line_subloop

			mpnl_line_wait: 

				; get line_wait amount
				inc hl
				ld a, (hl)
				ld channel.line_wait(ix), a
				jr mpnll_line_end

			mpnl_instruction_check_done: 
			
				inc hl
				jp music_process_new_line_subloop

			mpnll_line_end: 
				
				; end of this line reached
				; store the pointer to the next command and move to the next channel
				inc hl
				ld channel.pattern_ptr(ix), l
				ld channel.pattern_ptr+1(ix), h

                ld de, #_sizeof_channel
                add ix, de
            
				dec b
                jp nz, music_process_new_line_loop

                ; last channel processed, we're done
                ret 
