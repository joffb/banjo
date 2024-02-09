

; banjo sound driver
; Joe Kennedy 2023

banjo_update:

    push ix
    push iy

    ; check if a song is queued
    ld a, (queue_song)
    cp a, 0xff
    jr nz, _banjo_update_new_song

        ; song isn't queued, check if a song is playing
        ld a, (song_playing)
        or a, a
        jr z, _banjo_update_check_sfx

            ; if it is, run the updates
            ld ix, song_channels
            ld iy, song_state
            call music_update

            jr _banjo_update_check_sfx

    _banjo_update_new_song:

        ; new song has been queued
        call music_handle_song_queue

    _banjo_update_check_sfx:
    ld a, (queue_sfx)
    cp a, 0xff
    jr nz, _banjo_update_new_sfx

        ; sfx isn't queued, check if a sfx is playing
        ld a, (sfx_playing)
        or a, a
        jr z, _banjo_update_done

            ; if it is, run the updates
            ld ix, sfx_channel
            ld iy, sfx_state
            call music_update

            jr _banjo_update_done

    _banjo_update_new_sfx:

        ; new sfx has been queued
        call music_handle_sfx_queue

    _banjo_update_done:

    pop iy
    pop ix

    ret

; music update function to be called every frame
; ix: channels
; iy: music state
music_update:

	; switch bank to the one the song uses
	ld a, (iy + music_state.bank)
	ld (SLOT_2_BANK_CHANGE), a

	; clear process new line flag
	ld (iy + music_state.process_new_line), 0
	
	; inc subtic
	inc (iy + music_state.subtic)
	ld b, (iy + music_state.subtic)
	
	; check if subtic == time_base
	ld a, (iy + music_state.time_base)
	cp a, b
	jp nz, music_update_tics_done
	
		; wrap subtic around to 0
		ld (iy + music_state.subtic), 0
		
		; move to next tic
		inc (iy + music_state.tic)
		ld b, (iy + music_state.tic)
		
		; check if even or odd row
		ld a, (iy + music_state.line)
		and a, 0x1
		jr nz, mu_odd_row
		
			; even row
			ld a, (iy + music_state.speed_1)
			jr mu_speed_set
		
		mu_odd_row:
			
			; odd row
			ld a, (iy + music_state.speed_2)
			
		mu_speed_set:
		
		; check if we need to move to the next line
		cp a, b
		jr nz, music_update_tics_done
	
			; reset tic count
			ld (iy + music_state.tic), 0
	
			; move to next line
			inc (iy + music_state.line)
	
			; signal that we need to process the next line
			ld (iy + music_state.process_new_line), 1
	
			; check if we need to move to the next pattern
			ld b, (iy + music_state.line)
			ld a, (iy + music_state.pattern_length)
			cp a, b
			jr nz, music_update_tics_done
				
				; reset line count
				ld (iy + music_state.line), 0
				
				; move to next pattern in order
				inc (iy + music_state.order)
				ld b, (iy + music_state.order)
				
				; check if we've reached the last order
				ld a, (iy + music_state.orders_length)
				cp a, b
				jr nz, music_update_not_last_order
				
					; looping back to order 0
					ld (iy + music_state.order), 0
					
					; check whether this music is sfx or not
					ld a, (iy + music_state.sfx_channel)
					cp a, 0xff
					jr z, mu_not_sfx

						; check if this music is looping or not
						ld a, (iy + music_state.loop)
						or a, a
						jr z, mu_sfx_no_loop
					
						mu_sfx_loop:

							; return to start 
							push ix
							call music_reset_channels_order_and_pattern
							pop ix

							jr music_update_tics_done

						mu_sfx_no_loop:
							
							; done playing 
							ld a, 0
							ld (sfx_playing), a
						
							; update priority variable
							ld (sfx_priority), a
						
							; unmute sfx channel
							ld a, (iy + music_state.sfx_channel)
							call music_unmute_channel
						
							ret
						
					; if not, return to first order and pattern
					mu_not_sfx:

						; check if this music is looping or not
						ld a, (iy + music_state.loop)
						or a, a
						jr z, mu_song_no_loop
					
						mu_song_loop:

							; return to start 
							push ix
							call music_reset_channels_order_and_pattern
							pop ix

							jr music_update_tics_done

						mu_song_no_loop:
						
							; done playing 
							ld a, 0
							ld (song_playing), a

							; mute channels
							call music_mute_all_fm
							call music_mute_all_sn

							ret
				
				music_update_not_last_order:
				
					; update pattern to next in order
					call music_update_channels_order_and_pattern
	
	music_update_tics_done:
	
	; new line reached
	; preserve ix as it points to the first channel
	push ix
	ld a, (iy + music_state.process_new_line)
	or a, a
	call nz, music_process_new_line
	pop ix
	
	; clear process new line flag
	ld (iy + music_state.process_new_line), 0

	; process next tic (macros etc)
	call music_process_channels_tic
	
	ret


; reset all the channels' order and pattern pointers
; to the first order and pattern
; ix: channels
; iy: state
music_reset_channels_order_and_pattern:

	; get pointer to order_ptrs in hl
	ld l, (iy + music_state.order_ptrs)
	ld h, (iy + music_state.order_ptrs + 1)

	; loop through all channels
	ld b, (iy + music_state.channel_count)

	mrcoap_loop:
		
		; get address of first order into de
		; and update it in the channel
		ld e, (hl)
		ld (ix + channel.order_table_ptr), e
		inc hl
		ld d, (hl)
		ld (ix + channel.order_table_ptr + 1), d
		inc hl

		; using the order pointer, get the address
		; of the first pattern and update the channel
		ld a, (de)
		ld (ix + channel.pattern_ptr), a
		inc de
		ld a, (de)
		ld (ix + channel.pattern_ptr + 1), a

		; set line_wait = 0 for channel
		ld (ix + channel.line_wait), 0

		; move along to next channel
		ld de, _sizeof_channel
		add ix, de

		djnz mrcoap_loop

	ret


; move the order pointer along by one and update the pattern pointer
; to the pattern specified by the new order
; * relies on the channel struct being .order_table_ptr followed by .pattern_ptr 
; ix: channels
; iy: state
music_update_channels_order_and_pattern:

	ld b, (iy + music_state.channel_count)

    ; get channel pointer into hl
    push ix
    pop hl

    ; move it along to order_table_ptr
    ld de, channel.order_table_ptr
    add hl, de

	mucoap_loop:

		; get order pointer in de
		ld e, (hl)
        inc hl
		ld d, (hl)
        dec hl

		; advance to next order in table
		inc de
		inc de

		; store new order pointer in channel.order_table_ptr
		ld (hl), e
        inc hl
		ld (hl), d
        inc hl

		; get pattern pointer from (de) and store it in channel.pattern_ptr
		ld a, (de)
		ld (hl), a
		inc hl
        inc de
		ld a, (de)
		ld (hl), a

		; set line_wait = 0 for channel
		inc hl
		ld (hl), 0

		; move to next channel
		ld de, _sizeof_channel - 4
        add hl, de
		djnz mucoap_loop

	ret
