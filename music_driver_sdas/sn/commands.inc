
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; a: noise mode
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_sn_noise_mode: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command
	
	; preserve c
	ld l, c
	
	; update noise mode 
	ld c, channel.port(ix)
	out (c), a
	
	; restore c
	ld c, l
	
	; store for later
	ld music_state.noise_mode(iy), a
	
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
