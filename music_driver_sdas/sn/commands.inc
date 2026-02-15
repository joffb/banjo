
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_sn_noise_mode: 

	; don't update variable or flag a duty change if the noise mode is the same
	; to avoid resetting the lfsr
	ld a, (de)
	cp a, music_state.noise_mode(iy)
	jp z, mpnl_command_done
		
		; store for later w/ flag to say that the mode has changed
		or a, #0x80
		ld music_state.noise_mode(iy), a

		; flag that the noise mode has been changed
		set CHAN_EVENT_BIT_DUTY_CHANGED, channel.events(ix)
		
		jp mpnl_command_done

	
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_game_gear_pan: 

	; get pan value in l
	ld a, (de)
	ld l, a

	; get pan mask
	inc de
	ld a, (de)

	; mask off current music state pan value
	and a, music_state.panning(iy)

	; update with new pan value
	or a, l
	ld music_state.panning(iy), a
	ld l, a

	; check if we're in game gear mode
	ld a, (_banjo_system_flags)
	bit SYS_FLAG_BIT_GG, a
	jr z, mpnl_game_gear_pan_muted

		; update chip if this channel isn't muted
		bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
		jr nz, mpnl_game_gear_pan_muted
		
			ld a, l
			out (#GAME_GEAR_PAN_PORT), a

	mpnl_game_gear_pan_muted: 

	jp mpnl_command_done
