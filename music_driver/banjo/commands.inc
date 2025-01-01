
; banjo sound driver
; Joe Kennedy 2023

mpnl_instrument_change:
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

	; get new instrument
	inc de
	ld a, (de)

	; don't do anything if the instrument number is the same
	cp a, (ix + channel.instrument_num)
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
	ld (ix + channel.volume), a

	; set bit to indicate a channel volume change event
	set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

	inc hl
	jp mpnl_command_done


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
	jp mpnl_command_done

	
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
	jp mpnl_command_done


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
	jp mpnl_command_done


mpnl_slide_off:

	; restore original hl
	ex de, hl

	; clear slide
	ld (ix + channel.slide_amount), 0
	ld (ix + channel.slide_type), 0

	; clear slide flag
	res CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_arpeggio:

	; restore original hl
	ex de, hl

	; load arpeggio offsets
	inc hl
	ld a, (hl)
	ld (ix + channel.arpeggio), a

	; set arpeggio position to 3
	ld (ix + channel.arpeggio_pos), 3

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_arpeggio_off:

	; restore original hl
	ex de, hl

	; clear arpeggio
	ld (ix + channel.arpeggio_pos), 0
	ld (ix + channel.arpeggio), 0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_vibrato:

	; restore original hl
	ex de, hl

	; initialise vibrato counter
	ld (ix + channel.vibrato_counter), 0

	; get vibrato counter add amount
	inc hl
	ld a, (hl)
	ld (ix + channel.vibrato_counter_add), a
	
	; load full vibrato amplitude into vibrato_depth
	inc hl
	ld a, (hl)
	ld (ix + channel.vibrato_depth), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_vibrato_off:

	; restore original hl
	ex de, hl

	; clear vibrato flag
	res CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, (ix + channel.events)

	inc hl
	jp mpnl_command_done


mpnl_legato_on:

	; restore original hl
	ex de, hl

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_legato_off:

	; restore original hl
	ex de, hl

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	inc hl
	jp mpnl_command_done


mpnl_order_jump:

	; restore original hl
	ex de, hl

	; get order to jump to and store it
	inc hl
	ld a, (hl)
	ld (iy + music_state.order_jump), a

	inc hl
	jp mpnl_command_done


mpnl_order_next:

	; restore original hl
	ex de, hl

	; get order to jump to by incrementing to the current order
	ld a, (iy + music_state.order)
    inc a

    ; check it's a valid order
    cp a, (iy + music_state.orders_length)
    jr c, mpnlon_order_ok

        ld a, 0

    mpnlon_order_ok:
	ld (iy + music_state.order_jump), a

	inc hl
	jp mpnl_command_done


mpnl_set_speed_1:

	; restore original hl
	ex de, hl

	; get new speed
	inc hl
	ld a, (hl)
	ld (iy + music_state.speed_1), a

	inc hl
	jp mpnl_command_done


mpnl_set_speed_2:

	; restore original hl
	ex de, hl

	; get new speed
	inc hl
	ld a, (hl)
	ld (iy + music_state.speed_2), a

	inc hl
	jp mpnl_command_done


mpnl_note_delay:

	; restore original hl
	ex de, hl

	; get new note delay
	inc hl
	ld a, (hl)
	ld (ix + channel.tic_wait), a

	; store the pointer to the next command and return
	; this will be picked up when tic_wait hits zero
	inc hl
	ld (ix + channel.pattern_ptr), l
	ld (ix + channel.pattern_ptr + 1), h

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

