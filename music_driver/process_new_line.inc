
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

		; get current pattern pointer for this channel
		ld l, (ix + channel.pattern_ptr)
		ld h, (ix + channel.pattern_ptr + 1)
		
		music_process_new_line_subloop:
		
			; get command
			ld a, (hl)
			
            ; is the end of this line for this channel? (bit 7 set, a >= 0x80)
			; if not, use jump table
			bit 7, a
			jp z, mpnl_use_jump_table

				; end of this line reached
				; get line_wait amount and set it in channel
				and a, 0x7f
				ld (ix + channel.line_wait), a

				; store the pointer to the next command
				inc hl
				ld (ix + channel.pattern_ptr), l
				ld (ix + channel.pattern_ptr + 1), h	

				; move to next channel
				ld de, _sizeof_channel
				add ix, de

				; decrease b, start processing next channel if it's > 0
				djnz music_process_new_line_loop

				; last channel processed, we're done
				ret

			; if not, process the command
            mpnl_use_jump_table:

                ; preserve pattern pointer (currently in hl) in de
                ; we want to use hl's ability to jump
                ex de, hl

				; address in table is command num * 3
				ld c, a
				add a, a
				add a, c

                ; use it to offset into the jump table
                ld h, >music_process_jump_table 
                add a, <music_process_jump_table 
                ld l, a
				adc a, h
				sub a, l
				ld h, a

				; jump to jump table address
				; note we still need to restore hl from de after the jump!
				jp (hl)


; jumps to code to handle commands
music_process_jump_table:

	jp mpnl_note_on
	jp mpnl_note_off
	jp mpnl_instrument_change
	jp mpnl_volume_change
	jp mpnl_fm_drum
	jp mpnl_noise_mode
	jp mpnl_pitch_slide_up
	jp mpnl_pitch_slide_down
	jp mpnl_portamento
	jp mpnl_slide_off
	jp mpnl_arpeggio
	jp mpnl_arpeggio_off
	jp mpnl_vibrato
	jp mpnl_vibrato_off
	jp mpnl_legato_on
	jp mpnl_legato_off
	jp mpnl_game_gear_pan
	

; jumps to code to handle note-ons for different channel types
; aligned to 4 bytes
music_process_note_on_jump_table:

	jp music_sn_note_on
	nop
	jp music_fm_note_on
	nop
	jp music_fm_drums_note_on
	nop

; jumps to code to handle note-offs for different channel types
; aligned to 4 bytes
music_process_note_off_jump_table:

	jp music_sn_note_off
	nop
	jp music_fm_note_off
	nop
	jp music_fm_drums_note_off
	nop

; jumps to code to handle volume changes for different channel types
; aligned to 4 bytes
music_process_volume_change_jump_table:

	jp music_volume_change_sn
	nop
	jp music_volume_change_fm
	nop
	jp music_volume_change_fm_drums
	nop

mpnl_note_on:

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr z, mpnl_note_on_not_muted
	
		; restore original hl
		ex de, hl

		; don't do anything if channel muted
		inc hl
		inc hl
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_note_on_not_muted:

	; reset volume macro position and pointer if we have one
	ld a, (ix + channel.volume_macro_len)
	or a, a 
	jr z, music_process_new_line_no_volume_macro

	; don't reset the volume macro position if legato is on
	bit CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)
	jr nz, music_process_new_line_no_volume_macro

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
	set CHAN_FLAG_BIT_PITCH_CHANGED, (ix + channel.flags)

	; different note-on procedures per-channel type
	; they should all do
	;	jp music_process_new_line_subloop
	; when they're done

	; get channel_type * 4
	ld a, (ix + channel.type)
	add a, a
	add a, a

	; add to jump table
	ld h, >music_process_note_on_jump_table
	add a, <music_process_note_on_jump_table
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; jump to jump table
	; note we still need to restore hl from de after the jump!
	jp (hl)

mpnl_note_off:

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr z, mpnl_note_off_not_muted

		; restore original hl
		ex de, hl

		; don't do anything if channel muted
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_note_off_not_muted:
	
	; different note-off procedures per-channel type
	; they should all do
	;	jp music_process_new_line_subloop
	; when they're done

	; get channel_type * 4
	ld a, (ix + channel.type)
	add a, a
	add a, a

	; add to jump table
	ld h, >music_process_note_off_jump_table
	add a, <music_process_note_off_jump_table
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; jump to jump table
	; note we still need to restore hl from de after the jump!
	jp (hl)

mpnl_instrument_change:
			
	; restore original hl
	ex de, hl

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr z, mpnl_instrument_change_not_muted
	
		; don't do anything if it is
		inc hl
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_instrument_change_not_muted:

	; get current instrument
	ld a, (ix + channel.instrument_num)
	ld c, a

	; get new instrument
	inc hl
	ld a, (hl)

	; don't do anything if the instrument number is the same
	cp a, c
	jr z, mpnl_instrument_change_done

		; exchange de and hl so we can use "ld r, (hl)"" instructions
		ex de, hl
		
		; update the instrument
		call music_instrument_change

		; restore original hl
		ex de, hl

	mpnl_instrument_change_done:

	inc hl
	jp music_process_new_line_subloop

mpnl_volume_change:

	; get channel_type * 4
	ld a, (ix + channel.type)
	add a, a
	add a, a

	; add to jump table
	ld h, >music_process_volume_change_jump_table
	add a, <music_process_volume_change_jump_table
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; jump to jump table
	; note we still need to restore hl from de after the jump!
	jp (hl)

mpnl_fm_drum:

	; restore original hl
	ex de, hl

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr z, mpnl_fm_drum_not_muted
	
		; don't do anything if it is
		inc hl
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_fm_drum_not_muted:
	jp music_fm_drum_change

mpnl_noise_mode:

	; restore original hl
	ex de, hl

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
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

	; restore original hl
	ex de, hl

	; get slide amount and store it for later
	inc hl
	ld a, (hl)
	ld (ix + channel.slide_amount), a
	
	; set slide_type = 1
	ld (ix + channel.slide_type), SLIDE_TYPE_UP

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)
	
	inc hl				
	jp music_process_new_line_subloop
	
mpnl_pitch_slide_down:

	; restore original hl
	ex de, hl

	; get slide amount and store it for later
	inc hl
	ld a, (hl)
	ld (ix + channel.slide_amount), a
	
	; set slide_type = 2
	ld (ix + channel.slide_type), SLIDE_TYPE_DOWN

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)

	inc hl				
	jp music_process_new_line_subloop

mpnl_portamento:

	; restore original hl
	ex de, hl

	; load portamento amount
	inc hl
	ld a, (hl)
	ld (ix + channel.slide_amount), a

	; set slide_type = 3
	ld (ix + channel.slide_type), SLIDE_TYPE_PORTA

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_slide_off:

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld (ix + channel.slide_amount), 0
	ld (ix + channel.slide_type), 0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_arpeggio:

	; restore original hl
	ex de, hl

	; load arpeggio offsets
	inc hl
	ld a, (hl)
	ld (ix + channel.arpeggio), a

	; set arpeggio position to 0
	ld (ix + channel.arpeggio_pos), 0

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_arpeggio_off:

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld (ix + channel.arpeggio_pos), 0
	ld (ix + channel.arpeggio), 0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_vibrato:

	; restore original hl
	ex de, hl

	; initialise vibrato counter
	ld (ix + channel.vibrato_counter), 0

	; get vibrato counter add amount
	inc hl
	ld a, (hl)
	ld (ix + channel.vibrato_counter_add), a

	; if vibrato is already happening we don't want to reset the current vibrato amplitude
	; as it would effectively make the current freq into the new centre pitch
	bit CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)
	jr nz, mpnl_dont_reset_vibrato

		; initialise vibrato_current to VIBRATO_CENTRE (equivalent to 0 vibrato)
		ld (ix + channel.vibrato_current), VIBRATO_CENTRE

	mpnl_dont_reset_vibrato:

	; load full vibrato amplitude plus VIBRATO_CENTRE offset into vibrato_target
	inc hl
	ld a, (hl)
	add a, VIBRATO_CENTRE
	ld (ix + channel.vibrato_target), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop


mpnl_vibrato_off:

	; restore original hl
	ex de, hl

	; move past this instruction
	inc hl

	; check whether this channel has vibrato
	bit CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)
	jp z, music_process_new_line_subloop

		; clear vibrato flag
		res CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)

		; mark pitch as requiring update
		set CHAN_FLAG_BIT_PITCH_CHANGED, (ix + channel.flags)

		; get current vibrato offset and 
		; subtract VIBRATO_CENTRE so it's now a 
		; two's complement number
		ld a, (ix + channel.vibrato_current)
		sub a, VIBRATO_CENTRE

		; undo the vibrato offset to return to the original note
		; is the value positive or negative?
		bit 7, a
		jr nz, mpnl_vo_neg

			add a, (ix + channel.freq)
			ld (ix + channel.freq), a

			ld a, 0
			adc a, (ix + channel.freq + 1)
			ld (ix + channel.freq + 1), a

			jp music_process_new_line_subloop
		
		mpnl_vo_neg:

			neg
			ld c, a
			ld a, (ix + channel.freq)
			sub a, c
			ld (ix + channel.freq), a

			ld a, (ix + channel.freq + 1)
			sbc a, 0
			ld (ix + channel.freq + 1), a

			jp music_process_new_line_subloop


mpnl_legato_on:

	; restore original hl
	ex de, hl

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_legato_off:

	; restore original hl
	ex de, hl

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	inc hl
	jp music_process_new_line_subloop

mpnl_game_gear_pan:

	; restore original hl
	ex de, hl

	; get pan value
	inc hl
	ld c, (hl)

	; get pan mask
	inc hl
	ld a, (hl)

	; mask off current music state pan value
	and a, (iy + music_state.panning)

	; update with new pan value
	or a, c
	ld (iy + music_state.panning), a
	ld c, a

	; check if we're in game gear mode
	ld a, (banjo_game_gear_mode)
	cp a, 1
	jr nz, mpnl_game_gear_pan_muted

		; update chip if this channel isn't muted
		bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
		jr nz, mpnl_game_gear_pan_muted
		
			ld a, c
			out (GAME_GEAR_PAN_PORT), a

	mpnl_game_gear_pan_muted:

	inc hl
	jp music_process_new_line_subloop


mpnl_instruction_check_done:

	; restore original hl
	ex de, hl

	inc hl
	jp music_process_new_line_subloop

