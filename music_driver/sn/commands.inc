
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; a: noise mode
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_sn_noise_mode:

	; store for later
	ld (iy + music_state.noise_mode), a

	; flag that the noise mode has been changed
	set CHAN_EVENT_BIT_DUTY_CHANGED, (ix + channel.events)
	
	jp mpnl_command_done

	
; a: pan value
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_game_gear_pan:

	; get pan value in l
	ld l, a

	; get pan mask
	inc de
	ld a, (de)

	; mask off current music state pan value
	and a, (iy + music_state.panning)

	; update with new pan value
	or a, l
	ld (iy + music_state.panning), a
	ld l, a

	; check if we're in game gear mode
	ld a, (banjo_system_flags)
	bit SYS_FLAG_BIT_GG, a
	jr z, mpnl_game_gear_pan_muted

		; update chip if this channel isn't muted
		bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
		jr nz, mpnl_game_gear_pan_muted
		
			ld a, l
			out (GAME_GEAR_PAN_PORT), a

	mpnl_game_gear_pan_muted:

	jp mpnl_command_done
