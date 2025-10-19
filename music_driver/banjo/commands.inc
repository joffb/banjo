
; banjo sound driver
; Joe Kennedy 2023

; a: instrument number
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_instrument_change:
			
	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

	; don't do anything if the instrument number is the same
	cp a, (ix + channel.instrument_num)
	jr z, mpnl_instrument_change_done
		
		; update the instrument
		call music_instrument_change
		
	mpnl_instrument_change_done:

	jp mpnl_command_done

; a: new volume
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_volume_change:

	and a, 0xf

	cp a, (ix + channel.volume)
	jp z, mpnl_command_done

		; store new channel volume
		ld (ix + channel.volume), a

		; set bit to indicate a channel volume change event
		set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

		jp mpnl_command_done
	

.ifndef BANJO_MINIMAL

; a: slide amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_up:
	
	; set slide_type = 1
	ld (ix + channel.slide_type), SLIDE_TYPE_UP

	jr mpnl_slide_common

; a: slide amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_pitch_slide_down:
	
	; set slide_type = 2
	ld (ix + channel.slide_type), SLIDE_TYPE_DOWN

	jr mpnl_slide_common

; a: portamento amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_portamento:

	; set slide_type = 3
	ld (ix + channel.slide_type), SLIDE_TYPE_PORTA

mpnl_slide_common:

	; load slide amount
	ld (ix + channel.slide_amount), a

	; set pitch slide flag
	set CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, (ix + channel.events)

	jp mpnl_command_done


mpnl_slide_off:

	; clear slide
	;ld (ix + channel.slide_amount), 0
	ld (ix + channel.slide_type), 0

	; clear slide flag
	res CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, (ix + channel.events)

	; check if we need to clear the "pitch always changing" flag
	call music_refresh_pitch_change_always

	jp mpnl_command_done_one_byte_command


; a: arpeggio values
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio:

	; load arpeggio offsets
	ld (ix + channel.arpeggio), a

	; set arpeggio position to 3
	ld (ix + channel.arpeggio_pos), 3

	; set arpeggio flag
	set CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, (ix + channel.events)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_arpeggio_off:

	; clear arpeggio
	;ld (ix + channel.arpeggio_pos), 0
	;ld (ix + channel.arpeggio), 0

	; clear arpeggio flag
	res CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, (ix + channel.events)
	
	; check if we need to clear the "pitch always changing" flag
	call music_refresh_pitch_change_always

	jp mpnl_command_done_one_byte_command


; a: vibrato amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato:

	; initialise vibrato counter
	ld (ix + channel.vibrato_counter), 0

	; get vibrato counter add amount
	ld (ix + channel.vibrato_counter_add), a
	
	; load full vibrato amplitude into vibrato_depth
	inc de
	ld a, (de)
	ld (ix + channel.vibrato_depth), a

	; set vibrato flag
	set CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)
	
	; set "pitch always changing" flag
	set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, (ix + channel.events)

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_vibrato_off:

	; clear vibrato flag
	res CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)

	; mark pitch as requiring update
	set CHAN_EVENT_BIT_PITCH_CHANGED, (ix + channel.events)

	; check if we need to clear the "pitch always changing" flag
	call music_refresh_pitch_change_always

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_on:

	; switch legato flag on
	set CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_legato_off:

	; switch legato flag off
	res CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)

	jp mpnl_command_done_one_byte_command

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_order_jump:

	; get order to jump to and store it
	ld (iy + music_state.order_jump), a

	jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_order_next:

	; get order to jump to by incrementing to the current order
	ld a, (iy + music_state.order)
    inc a

    ; check it's a valid order
    cp a, (iy + music_state.orders_length)
    jr c, mpnlon_order_ok

        ld a, 0

    mpnlon_order_ok:
	ld (iy + music_state.order_jump), a

	jp mpnl_command_done_one_byte_command


; a: new speed
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_1:

	; get new speed
	ld (iy + music_state.speed_1), a

	jp mpnl_command_done


; a: new speed
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_set_speed_2:

	; get new speed
	ld (iy + music_state.speed_2), a

	jp mpnl_command_done


; a: delay amount
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_note_delay:

	; get new note delay
	ld (ix + channel.tic_wait), a

	; store the pointer to the next command and return
	; this will be picked up when tic_wait hits zero
	inc de
	ld (ix + channel.pattern_ptr), e
	ld (ix + channel.pattern_ptr + 1), d

	ret

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_note_release:

	set CHAN_EVENT_BIT_MACRO_RELEASE, (ix + channel.events)

	jp mpnl_command_done_one_byte_command


music_refresh_pitch_change_always:

	; any of these effects running?
	ld a, (ix + channel.flags)
	and a, CHAN_FLAG_BIT_VIBRATO | CHAN_FLAG_ARPEGGIO | CHAN_FLAG_SLIDE
	jr nz, mrpca_set

		; clear PITCH_CHANGE_ALWAYS flag
		res CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, (ix + channel.events)
		ret
	
	; set PITCH_CHANGE_ALWAYS flag
	mrpca_set:
	
		set CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS, (ix + channel.events)
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

