
; banjo sound driver
; Joe Kennedy 2023

; a: instrument number
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_instrument_change: 
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

	; don't do anything if the instrument number is the same
	cp a, channel.instrument_num(ix)
	jr z, mpnl_instrument_change_done
		
		; update the instrument
		call music_instrument_change
		
	mpnl_instrument_change_done: 

	jp mpnl_command_done

; a: new volume
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_volume_change: 

	and a, #0xf

	; store new channel volume
	ld channel.volume(ix), a

	; set bit to indicate a channel volume change event
	set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

	jp mpnl_command_done
	

.ifndef BANJO_MINIMAL

; a: slide amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_up: 
	
	; set slide_type = 1
	ld channel.slide_type(ix), #SLIDE_TYPE_UP

	jr mpnl_slide_common

; a: slide amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_down: 
	
	; set slide_type = 2
	ld channel.slide_type(ix), #SLIDE_TYPE_DOWN

	jr mpnl_slide_common

; a: portamento amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_portamento: 

	; set slide_type = 3
	ld channel.slide_type(ix), #SLIDE_TYPE_PORTA

mpnl_slide_common: 

	; load portamento amount
	ld channel.slide_amount(ix), a

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	jp mpnl_command_done


mpnl_slide_off: 

	; clear slide
	;ld (ix + channel.slide_amount), 0
	ld channel.slide_type(ix), #0

	; clear slide flag
	res CHAN_FLAG_BIT_SLIDE, channel.flags(ix)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	jp mpnl_command_done_one_byte_command


; a: arpeggio values
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio: 

	; load arpeggio offsets
	ld channel.arpeggio(ix), a

	; set arpeggio position to 3
	ld channel.arpeggio_pos(ix), #3

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio_off: 

	; clear arpeggio
	;ld (ix + channel.arpeggio_pos), 0
	;ld (ix + channel.arpeggio), 0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	jp mpnl_command_done_one_byte_command


; a: vibrato amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato: 

	; initialise vibrato counter
	ld channel.vibrato_counter(ix), #0

	; get vibrato counter add amount
	ld channel.vibrato_counter_add(ix), a
	
	; load full vibrato amplitude into vibrato_depth
	inc de
	ld a, (de)
	ld channel.vibrato_depth(ix), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato_off: 

	; clear vibrato flag
	res CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_on: 

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_off: 

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, channel.flags(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_order_jump: 

	; get order to jump to and store it
	ld music_state.order_jump(iy), a

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_order_next: 

	; get order to jump to by incrementing to the current order
	ld a, music_state.order(iy)
    inc a

    ; check it's a valid order
    cp a, music_state.orders_length(iy)
    jr c, mpnlon_order_ok

        ld a, #0

    mpnlon_order_ok: 
	ld music_state.order_jump(iy), a

	jp mpnl_command_done_one_byte_command


; a: new speed
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_1: 

	; get new speed
	ld music_state.speed_1(iy), a

	jp mpnl_command_done


; a: new speed
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_2: 

	; get new speed
	ld music_state.speed_2(iy), a

	jp mpnl_command_done


; a: delay amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_note_delay: 

	; get new note delay
	ld channel.tic_wait(ix), a

	; store the pointer to the next command and return
	; this will be picked up when tic_wait hits zero
	inc de
	ld channel.pattern_ptr(ix), e
	ld channel.pattern_ptr+1(ix), d

	ret 


.endif 

.ifdef BANJO_MINIMAL
mpnl_arpeggio_off: 
mpnl_slide_off: 
mpnl_vibrato_off: 
mpnl_legato_on: 
mpnl_legato_off: 
mpnl_order_next: 
.endif 

; skip commands of various sizes
; used for channel types which don't support a command
mpnl_skip_1_byte_command: 

	jp mpnl_command_done_one_byte_command


.ifdef BANJO_MINIMAL
mpnl_pitch_slide_up: 
mpnl_pitch_slide_down: 
mpnl_portamento: 
mpnl_arpeggio: 
mpnl_order_jump: 
mpnl_set_speed_1: 
mpnl_set_speed_2: 
mpnl_note_delay: 
.endif 

mpnl_skip_2_byte_command: 

	jp mpnl_command_done


.ifdef BANJO_MINIMAL
mpnl_vibrato: 
.endif 

mpnl_skip_3_byte_command: 

	inc de

	jp mpnl_command_done

