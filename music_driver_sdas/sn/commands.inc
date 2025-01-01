
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

mpnl_sn_noise_mode: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

	; restore original hl
	ex de, hl
	
	ld c, channel.port(ix)

	; update noise mode 
	inc hl
	ld a, (hl)
	out (c), a
	
	; store for later
	ld music_state.noise_mode(iy), a
	
	inc hl
	jp mpnl_command_done


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
	ld a, (_banjo_system_flags)
	bit SYS_FLAG_BIT_GG, a
	jr z, mpnl_game_gear_pan_muted

		; update chip if this channel isn't muted
		bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
		jr nz, mpnl_game_gear_pan_muted
		
			ld a, c
			out (#GAME_GEAR_PAN_PORT), a

	mpnl_game_gear_pan_muted: 

	inc hl
	jp mpnl_command_done
