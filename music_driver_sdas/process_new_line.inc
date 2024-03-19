
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

		; get current pattern pointer for this channel
		ld l, channel.pattern_ptr(ix)
		ld h, channel.pattern_ptr+1(ix)
		
		music_process_new_line_subloop: 
		
			; get command
			ld a, (hl)
			
            ; is the end of this line for this channel? (bit 7 set, a >= 0x80)
			; if not, use jump table
			bit 7, a
			jp z, mpnl_use_jump_table

				; end of this line reached
				; get line_wait amount and set it in channel
				and a, #0x7f
				ld channel.line_wait(ix), a

				; store the pointer to the next command
				inc hl
				ld channel.pattern_ptr(ix), l
				ld channel.pattern_ptr+1(ix), h

				; move to next channel
				ld de, #_sizeof_channel
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
                ld h, #>music_process_jump_table
                add a, #<music_process_jump_table
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
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr z, mpnl_note_on_not_muted
	
		; restore original hl
		ex de, hl

		; don't do anything if channel muted
		inc hl
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_note_on_not_muted: 

	; reset volume macro position and pointer if we have one
	ld a, channel.volume_macro_len(ix)
	or a, a
	jr z, music_process_new_line_no_volume_macro

	; don't reset the volume macro position if legato is on
	bit CHAN_FLAG_BIT_LEGATO, channel.flags(ix)
	jr nz, music_process_new_line_no_volume_macro

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
	set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

	; different note-on procedures per-channel type
	; they should all do
	;	jp music_process_new_line_subloop
	; when they're done

	; get channel_type * 4
	ld a, channel.type(ix)
	add a, a
	add a, a

	; add to jump table
	ld h, #>music_process_note_on_jump_table
	add a, #<music_process_note_on_jump_table
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; jump to jump table
	; note we still need to restore hl from de after the jump!
	jp (hl)

mpnl_note_off: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
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
	ld a, channel.type(ix)
	add a, a
	add a, a

	; add to jump table
	ld h, #>music_process_note_off_jump_table
	add a, #<music_process_note_off_jump_table
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
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr z, mpnl_instrument_change_not_muted
	
		; don't do anything if it is
		inc hl
		inc hl
		jp music_process_new_line_subloop
	
	mpnl_instrument_change_not_muted: 

	; get current instrument
	ld a, channel.instrument_num(ix)
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
	ld a, channel.type(ix)
	add a, a
	add a, a

	; add to jump table
	ld h, #>music_process_volume_change_jump_table
	add a, #<music_process_volume_change_jump_table
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
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
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
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
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

	; restore original hl
	ex de, hl

	; get slide amount and store it for later
	inc hl
	ld a, (hl)
	ld channel.slide_amount(ix), a
	
	; set slide_type = 1
	ld channel.slide_type(ix), #SLIDE_TYPE_UP

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, channel.flags(ix)
	
	inc hl
	jp music_process_new_line_subloop
	
mpnl_pitch_slide_down: 

	; restore original hl
	ex de, hl

	; get slide amount and store it for later
	inc hl
	ld a, (hl)
	ld channel.slide_amount(ix), a
	
	; set slide_type = 2
	ld channel.slide_type(ix), #SLIDE_TYPE_DOWN

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_portamento: 

	; restore original hl
	ex de, hl

	; load portamento amount
	inc hl
	ld a, (hl)
	ld channel.slide_amount(ix), a

	; set slide_type = 3
	ld channel.slide_type(ix), #SLIDE_TYPE_PORTA

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_slide_off: 

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld channel.slide_amount(ix), #0
	ld channel.slide_type(ix), #0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_arpeggio: 

	; restore original hl
	ex de, hl

	; load arpeggio offsets
	inc hl
	ld a, (hl)
	ld channel.arpeggio(ix), a

	; set arpeggio position to 0
	ld channel.arpeggio_pos(ix), #0

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_arpeggio_off: 

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld channel.arpeggio_pos(ix), #0
	ld channel.arpeggio(ix), #0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_vibrato: 

	; restore original hl
	ex de, hl

	; initialise vibrato counter
	ld channel.vibrato_counter(ix), #0

	; get vibrato counter add amount
	inc hl
	ld a, (hl)
	ld channel.vibrato_counter_add(ix), a

	; if vibrato is already happening we don't want to reset the current vibrato amplitude
	; as it would effectively make the current freq into the new centre pitch
	bit CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)
	jr nz, mpnl_dont_reset_vibrato

		; initialise vibrato_current to VIBRATO_CENTRE (equivalent to 0 vibrato)
		ld channel.vibrato_current(ix), #VIBRATO_CENTRE

	mpnl_dont_reset_vibrato: 

	; load full vibrato amplitude plus VIBRATO_CENTRE offset into vibrato_target
	inc hl
	ld a, (hl)
	add a, #VIBRATO_CENTRE
	ld channel.vibrato_target(ix), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop


mpnl_vibrato_off: 

	; restore original hl
	ex de, hl

	; move past this instruction
	inc hl

	; check whether this channel has vibrato
	bit CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)
	jp z, music_process_new_line_subloop

		; clear vibrato flag
		res CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

		; mark pitch as requiring update
		set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

		; get current vibrato offset and 
		; subtract VIBRATO_CENTRE so it's now a 
		; two's complement number
		ld a, channel.vibrato_current(ix)
		sub a, #VIBRATO_CENTRE

		; undo the vibrato offset to return to the original note
		; is the value positive or negative?
		bit 7, a
		jr nz, mpnl_vo_neg

			add a, channel.freq(ix)
			ld channel.freq(ix), a

			ld a, #0
			adc a, channel.freq+1(ix)
			ld channel.freq+1(ix), a

			jp music_process_new_line_subloop
		
		mpnl_vo_neg: 

			neg 
			ld c, a
			ld a, channel.freq(ix)
			sub a, c
			ld channel.freq(ix), a

			ld a, channel.freq+1(ix)
			sbc a, #0
			ld channel.freq+1(ix), a

			jp music_process_new_line_subloop


mpnl_legato_on: 

	; restore original hl
	ex de, hl

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

	inc hl
	jp music_process_new_line_subloop

mpnl_legato_off: 

	; restore original hl
	ex de, hl

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

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
	and a, music_state.panning(iy)

	; update with new pan value
	or a, c
	ld music_state.panning(iy), a
	ld c, a

	; check if we're in game gear mode
	ld a, (_banjo_game_gear_mode)
	cp a, #1
	jr nz, mpnl_game_gear_pan_muted

		; update chip if this channel isn't muted
		bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
		jr nz, mpnl_game_gear_pan_muted
		
			ld a, c
			out (#GAME_GEAR_PAN_PORT), a

	mpnl_game_gear_pan_muted: 

	inc hl
	jp music_process_new_line_subloop


mpnl_instruction_check_done: 

	; restore original hl
	ex de, hl

	inc hl
	jp music_process_new_line_subloop

