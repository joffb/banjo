
; banjo sound driver
; Joe Kennedy 2023


_banjo_jp_hl: 
    jp (hl)

; update the song if one is playing
_banjo_update_song: 

	; check if a song is playing
	; return if it's not
	ld a, (song_playing)
	or a, a
	ret z

		; if it is, run the updates
		push ix
		push iy

		ld ix, #_song_channels
		ld iy, #_song_state
		call music_update

		pop iy
		pop ix

		ret 

; update function to be called every frame
; ix: channels
; iy: music state
music_update: 

	; move to next tic
	dec music_state.tic(iy)
	
	; check if we need to move to the next line
	jr nz, music_update_tics_done

		; signal that we need to process the next line
		set STATE_FLAG_BIT_PROCESS_NEW_LINE, music_state.flags(iy)

		; check if we need to jump to a new order
		bit STATE_FLAG_BIT_ORDER_JUMP, music_state.flags(iy)
		jr z, music_update_no_jump

			call music_jump_channels_order_and_pattern
		
			jr music_update_tics_done

		music_update_no_jump: 

		; move to next line
		dec music_state.line(iy)

		; line count is 0 means a new pattern
		jr z, music_update_pattern_done

			; on to next line in pattern
			ld a, music_state.line(iy)
			and a, #0x1
			jr nz, music_update_new_line_odd_speed

				ld a, music_state.speed_1(iy)
				jr music_update_new_line_speed_done

			music_update_new_line_odd_speed: 

				ld a, music_state.speed_2(iy)

			music_update_new_line_speed_done: 

			; update tic count with speed
			ld music_state.tic(iy), a

			; done for now
			jr music_update_tics_done

		music_update_pattern_done: 

			ld a, music_state.order(iy)
			inc a

			; check if we've reached the last order
			cp a, music_state.orders_length(iy)
			jr z, mu_last_order
			
				; not the last order
				; move on to next pattern
				call music_jump_channels_order_and_pattern
				jr music_update_tics_done
			
			; we've reached the last order
			mu_last_order: 

				; check if this music is looping or not
				bit STATE_FLAG_BIT_LOOP, music_state.flags(iy)
				jr z, mu_no_loop

					; the music does loop - so go back to first patterns
					xor a, a
					call music_jump_channels_order_and_pattern
					jr music_update_tics_done
				
				mu_no_loop: 

					; get pointer to song stop call
					ld l, music_state.song_stop(iy)
					ld h, music_state.song_stop+1(iy)

					; run call
					jp (hl)
			
	music_update_tics_done: 

		.ifndef BANJO_MINIMAL

		; check if there's a fade occurring
		ld a, music_state.master_volume_fade(iy)
		or a, a
		call nz, _banjo_update_fade

		.endif 

		; get pointer to channel update call
		ld l, music_state.channel_update_call(iy)
		ld h, music_state.channel_update_call+1(iy)

		; do call
		call _banjo_jp_hl

		; clear process new line flag
		res STATE_FLAG_BIT_PROCESS_NEW_LINE, music_state.flags(iy)

		; clear master volume change flag
		res STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, music_state.flags(iy)

		ret 
