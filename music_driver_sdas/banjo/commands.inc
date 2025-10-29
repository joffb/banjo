
; banjo sound driver
; Joe Kennedy 2023

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_instrument_change: 
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

		; don't do anything if the instrument number is the same
		ld a, (de)
		cp a, channel.instrument_num(ix)
		call nz, music_instrument_change

		jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_volume_change: 

	; get new volume
	ld a, (de)
	and a, #0xf

	; don't do anything if the volume is the same
	cp a, channel.volume(ix)
	jp z, mpnl_command_done

		; store new channel volume
		ld channel.volume(ix), a

		; set bit to indicate a channel volume change event
		set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

		jp mpnl_command_done
	

.ifndef BANJO_MINIMAL

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_up: 
	
	; set slide_type = 1
	ld channel.effect1(ix), #SLIDE_TYPE_UP

	jr mpnl_slide_common

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_down: 
	
	; set slide_type = 2
	ld channel.effect1(ix), #SLIDE_TYPE_DOWN

	jr mpnl_slide_common

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_portamento: 

	; set slide_type = 3
	ld channel.effect1(ix), #SLIDE_TYPE_PORTA

mpnl_slide_common: 

	; load slide amount
	ld a, (de)
	ld channel.effect2(ix), a

	; set pitch slide flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	or a, #CHAN_FLAG_SLIDE
	ld channel.flags(ix), a
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done


mpnl_slide_off: 

	; clear slide
	ld channel.effect1(ix), #0

	; clear slide flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	ld channel.flags(ix), a

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	; clear the "pitch always changing" flag
	res CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done_one_byte_command


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio: 

	; keep arp values
	ld a, (de)
	ld l, a

	; load arpeggio offsets
	rrca 
	rrca 
	rrca 
	rrca 
	and a, #0x0f
	ld channel.effect2(ix), a

	ld a, l
	and a, #0x0f
	ld channel.effect3(ix), a

	; set arpeggio position to 2
	ld channel.effect1(ix), #2

	; set arpeggio flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	or a, #CHAN_FLAG_ARPEGGIO
	ld channel.flags(ix), a
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio_off: 

	; clear arpeggio flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	ld channel.flags(ix), a

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)
	
	; clear the "pitch always changing" flag
	res CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done_one_byte_command


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato: 

	; initialise vibrato counter
	ld channel.effect1(ix), #0

	; get vibrato counter add amount
	ld a, (de)
	ld channel.effect2(ix), a
	
	; load full vibrato amplitude into vibrato_depth
	inc de
	ld a, (de)
	ld channel.effect3(ix), a

	; set vibrato flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	or a, #CHAN_FLAG_VIBRATO
	ld channel.flags(ix), a
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato_off: 

	; clear vibrato flag
	ld a, channel.flags(ix)
	and a, #~(CHAN_FLAG_ARPEGGIO|CHAN_FLAG_VIBRATO|CHAN_FLAG_SLIDE)
	ld channel.flags(ix), a

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, channel.events(ix)

	; clear the "pitch always changing" flag
	res CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, channel.events(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_on: 

	; switch legato flag on
	set CHAN_EVENT_BIT_LEGATO, channel.events(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_off: 

	; switch legato flag off
	res CHAN_EVENT_BIT_LEGATO, channel.events(ix)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_order_jump: 

	; get order to jump to and store it
	ld a, (de)
	ld music_state.order_jump(iy), a
	set STATE_FLAG_BIT_ORDER_JUMP, music_state.flags(iy)
	
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

        xor a, a

    mpnlon_order_ok: 
	ld music_state.order_jump(iy), a
	set STATE_FLAG_BIT_ORDER_JUMP, music_state.flags(iy)

	jp mpnl_command_done_one_byte_command


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_1: 

	; get new speed
	ld a, (de)
	ld music_state.speed_1(iy), a

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_2: 

	; get new speed
	ld a, (de)
	ld music_state.speed_2(iy), a

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_note_delay: 

	; get new note delay
	ld a, (de)
	ld channel.tic_wait(ix), a

	; store the pointer to the next command and return
	; this will be picked up when tic_wait hits zero
	inc de
	ld channel.pattern_ptr(ix), e
	ld channel.pattern_ptr+1(ix), d

	ret 

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_note_release: 

	set CHAN_EVENT_BIT_MACRO_RELEASE, channel.events(ix)

	jp mpnl_command_done_one_byte_command

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

