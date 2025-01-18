
; banjo sound driver
; Joe Kennedy 2023


; update the song if one is playing
banjo_update_song:

	; check if a song is playing
	; return if it's not
	ld a, (song_playing)
	or a, a
	ret z

		; if it is, run the updates
		push ix
		push iy

		ld ix, song_channels
		ld iy, song_state
		call music_update

		pop iy
		pop ix

		ret

; update function to be called every frame
; ix: channels
; iy: music state
music_update:

	; point hl at music_state.subtic
	push iy
	pop hl
	ld de, music_state.subtic
	add hl, de

	; inc subtic
	inc (hl)
	ld a, (hl)
	
	; check if subtic == time_base
	cp a, (iy + music_state.time_base)
	jp nz, music_update_tics_done
	
		; wrap subtic around to 0
		ld (hl), 0

		; point hl at music_state.tic
		inc hl
		
		; move to next tic
		inc (hl)
		
		; check if even or odd row
		bit 0, (iy + music_state.line)
		jr nz, mu_odd_row
		
			; even row
			ld a, (iy + music_state.speed_1)
			jr mu_speed_set
		
		mu_odd_row:
			
			; odd row
			ld a, (iy + music_state.speed_2)
			
		mu_speed_set:
		
		; check if we need to move to the next line
		cp a, (hl)
		jr nz, music_update_tics_done
	
			; reset tic count
			ld (hl), 0

			; point hl at music_state.line
			inc hl
	
			; move to next line
			inc (hl)
	
			; signal that we need to process the next line
			set STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
	
			; check if we need to jump to a new order
			ld a, (iy + music_state.order_jump)
			cp a, 0xff
			call nz, music_jump_channels_order_and_pattern

			; check if we need to move to the next pattern
			ld a, (iy + music_state.pattern_length)
			cp a, (hl)
			jr nz, music_update_tics_done
				
				; reset line count
				ld (hl), 0
				
				; point hl at music_state.order
				inc hl

				; move to next pattern in order
				inc (hl)

				; check if we've reached the last order
				ld a, (iy + music_state.orders_length)
				cp a, (hl)
				jr z, mu_last_order
				
					; not the last order
					; move on to next pattern
					call music_update_channels_order_and_pattern
					jr music_update_tics_done
				
				; we've reached the last order
				mu_last_order:

					; check if this music is looping or not
					bit STATE_FLAG_BIT_LOOP, (iy + music_state.flags)
					jr z, mu_no_loop

						; the music does loop - so go back to first patterns
						call music_reset_channels_order_and_pattern
						jr music_update_tics_done
					
					mu_no_loop:

						; get pointer to song stop call
						ld l, (iy + music_state.song_stop)
						ld h, (iy + music_state.song_stop + 1)

						; run call
						jp (hl)
			
	music_update_tics_done:

		.ifndef BANJO_MINIMAL

		; check if there's a fade occurring
		ld a, (iy + music_state.master_volume_fade)
		or a, a
		call nz, banjo_update_fade

		.endif

		; address for call to `ret` to
		ld hl, music_update_call_done
		push hl

		; get pointer to channel update call
		ld l, (iy + music_state.channel_update_call)
		ld h, (iy + music_state.channel_update_call + 1)

		; run call
		jp (hl)

		; then return here
		music_update_call_done:

		; clear process new line flag
		res STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)

		; clear master volume change flag
		res STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)

		ret