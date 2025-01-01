
; banjo sound driver
; Joe Kennedy 2023

mpnl_instrument_change: 
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

	; get new instrument
	inc de
	ld a, (de)

	; don't do anything if the instrument number is the same
	cp a, channel.instrument_num(ix)
	jr z, mpnl_instrument_change_done
		
		; update the instrument
		call music_instrument_change

	mpnl_instrument_change_done: 

	; restore original hl
	ex de, hl

	inc hl
	jp mpnl_command_done


mpnl_volume_change: 

	; restore original hl
	ex de, hl

	; store new channel volume
	inc hl
	ld a, (hl)
	ld channel.volume(ix), a

	; set bit to indicate a channel volume change event
	set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

	inc hl
	jp mpnl_command_done


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
	jp mpnl_command_done

	
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
	jp mpnl_command_done


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
	jp mpnl_command_done


mpnl_slide_off: 

	; restore original hl
	ex de, hl

	; clear slide
	ld channel.slide_amount(ix), #0
	ld channel.slide_type(ix), #0

	; clear slide flag
	res CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_arpeggio: 

	; restore original hl
	ex de, hl

	; load arpeggio offsets
	inc hl
	ld a, (hl)
	ld channel.arpeggio(ix), a

	; set arpeggio position to 3
	ld channel.arpeggio_pos(ix), #3

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_arpeggio_off: 

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld channel.arpeggio_pos(ix), #0
	ld channel.arpeggio(ix), #0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_vibrato: 

	; restore original hl
	ex de, hl

	; initialise vibrato counter
	ld channel.vibrato_counter(ix), #0

	; get vibrato counter add amount
	inc hl
	ld a, (hl)
	ld channel.vibrato_counter_add(ix), a
	
	; load full vibrato amplitude into vibrato_depth
	inc hl
	ld a, (hl)
	ld channel.vibrato_depth(ix), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_vibrato_off: 

	; restore original hl
	ex de, hl

	; clear vibrato flag
	res CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	inc hl
	jp mpnl_command_done


mpnl_legato_on: 

	; restore original hl
	ex de, hl

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_legato_off: 

	; restore original hl
	ex de, hl

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

	inc hl
	jp mpnl_command_done


mpnl_order_jump: 

	; restore original hl
	ex de, hl

	; get order to jump to and store it
	inc hl
	ld a, (hl)
	ld music_state.order_jump(iy), a

	inc hl
	jp mpnl_command_done


mpnl_order_next: 

	; restore original hl
	ex de, hl

	; get order to jump to by incrementing to the current order
	ld a, music_state.order(iy)
    inc a

    ; check it's a valid order
    cp a, music_state.orders_length(iy)
    jr c, mpnlon_order_ok

        ld a, #0

    mpnlon_order_ok: 
	ld music_state.order_jump(iy), a

	inc hl
	jp mpnl_command_done


mpnl_set_speed_1: 

	; restore original hl
	ex de, hl

	; get new speed
	inc hl
	ld a, (hl)
	ld music_state.speed_1(iy), a

	inc hl
	jp mpnl_command_done


mpnl_set_speed_2: 

	; restore original hl
	ex de, hl

	; get new speed
	inc hl
	ld a, (hl)
	ld music_state.speed_2(iy), a

	inc hl
	jp mpnl_command_done


mpnl_note_delay: 

	; restore original hl
	ex de, hl

	; get new note delay
	inc hl
	ld a, (hl)
	ld channel.tic_wait(ix), a

	; store the pointer to the next command and return
	; this will be picked up when tic_wait hits zero
	inc hl
	ld channel.pattern_ptr(ix), l
	ld channel.pattern_ptr+1(ix), h

	; remove jump table address from stack
	pop de

	ret 


; skip commands of various sizes
; used for channel types which don't support a command
mpnl_skip_1_byte_command: 

	; restore original hl
	ex de, hl

	inc hl
	
	jp mpnl_command_done


mpnl_skip_2_byte_command: 

	; restore original hl
	ex de, hl

	inc hl
	inc hl

	jp mpnl_command_done


mpnl_skip_3_byte_command: 

	; restore original hl
	ex de, hl

	inc hl
	inc hl
	inc hl

	jp mpnl_command_done

