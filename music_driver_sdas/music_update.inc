
; banjo sound driver
; Joe Kennedy 2023

.ifndef QUEUES_OFF

_banjo_update: 

    push ix
    push iy

    ; check if a song is queued
    ld a, (queue_song)
    cp a, #0xff
    jr nz, banjo_update_new_song

        ; song isn't queued, check if a song is playing
        ld a, (song_playing)
        or a, a
        jr z, banjo_update_check_sfx

            ; if it is, run the updates
            ld ix, #_song_channels
            ld iy, #_song_state
            call music_update

            jr banjo_update_check_sfx

    banjo_update_new_song: 

        ; new song has been queued
        call music_handle_song_queue

	banjo_update_check_sfx: 

	; check if an sfx is queued
    ld a, (queue_sfx)
    cp a, #0xff
    jr nz, banjo_update_new_sfx

        ; sfx isn't queued, check if a sfx is playing
        ld a, (sfx_playing)
        or a, a
        jr z, banjo_update_done

            ; if it is, run the updates
            ld ix, #_sfx_channel
            ld iy, #_sfx_state
            call music_update

            jr banjo_update_done

    banjo_update_new_sfx: 

        ; new sfx has been queued
        call music_handle_sfx_queue

    banjo_update_done: 

    pop iy
    pop ix

    ret 

.else 

_banjo_update: 
	ret 

.endif 

; update the song if one is playing
_banjo_update_song: 

	; check if a song is playing
	ld a, (song_playing)
	or a, a
	jr z, banjo_update_song_done

		push ix
		push iy

		; if it is, run the updates
		ld ix, #_song_channels
		ld iy, #_song_state
		call music_update

		pop iy
		pop ix

	banjo_update_song_done: 
	ret 

; update the sfx if one is playing
_banjo_update_sfx: 

	; check if a sfx is playing
	ld a, (sfx_playing)
	or a, a
	jr z, banjo_update_sfx_done

		push ix
		push iy

		; if it is, run the updates
		ld ix, #_sfx_channel
		ld iy, #_sfx_state
		call music_update

		pop iy
		pop ix

	banjo_update_sfx_done: 
	ret 

; update function to be called every frame
; ix: channels
; iy: music state
music_update: 

	.ifndef QUEUES_OFF
	; switch bank to the one the song uses
	ld a, music_state.bank(iy)
	ld (SLOT_2_BANK_CHANGE), a
	.endif 

	; clear process new line flag
	ld music_state.process_new_line(iy), #0
	
	; inc subtic
	inc music_state.subtic(iy)
	ld a, music_state.subtic(iy)
	
	; check if subtic == time_base
	cp a, music_state.time_base(iy)
	jp nz, music_update_tics_done
	
		; wrap subtic around to 0
		ld music_state.subtic(iy), #0
		
		; move to next tic
		inc music_state.tic(iy)
		ld b, music_state.tic(iy)
		
		; check if even or odd row
		bit 0, music_state.line(iy)
		jr nz, mu_odd_row
		
			; even row
			ld a, music_state.speed_1(iy)
			jr mu_speed_set
		
		mu_odd_row: 
			
			; odd row
			ld a, music_state.speed_2(iy)
			
		mu_speed_set: 
		
		; check if we need to move to the next line
		cp a, b
		jr nz, music_update_tics_done
	
			; reset tic count
			ld music_state.tic(iy), #0
	
			; move to next line
			inc music_state.line(iy)
	
			; signal that we need to process the next line
			ld music_state.process_new_line(iy), #1
	
			; check if we need to move to the next pattern
			ld a, music_state.pattern_length(iy)
			cp a, music_state.line(iy)
			jr nz, music_update_tics_done
				
				; reset line count
				ld music_state.line(iy), #0
				
				; move to next pattern in order
				inc music_state.order(iy)

				; check if we've reached the last order
				ld a, music_state.orders_length(iy)
				cp a, music_state.order(iy)
				jr nz, music_update_not_last_order
				
					; looping back to order 0
					ld music_state.order(iy), #0
					
					; check whether this music is sfx or not
					ld a, music_state.sfx_channel(iy)
					cp a, #0xff
					jr z, mu_not_sfx

						; check if this music is looping or not
						ld a, music_state.loop(iy)
						or a, a
						jr z, mu_sfx_no_loop
					
						mu_sfx_loop: 

							; return to start 
							call music_reset_channels_order_and_pattern

							jr music_update_tics_done

						mu_sfx_no_loop: 
							
							; done playing 
							ld a, #0
							ld (sfx_playing), a
						
							; update priority variable
							ld (sfx_priority), a
						
							; mute sfx channel
							push ix
							pop hl
							call music_mute_channel

							; unmute song channel
							ld a, music_state.sfx_channel(iy)
							call _banjo_unmute_song_channel
						
							ret 
						
					; if not, return to first order and pattern
					mu_not_sfx: 

						; check if this music is looping or not
						ld a, music_state.loop(iy)
						or a, a
						jr z, mu_song_no_loop
					
						mu_song_loop: 

							; return to start 
							call music_reset_channels_order_and_pattern

							jr music_update_tics_done

						mu_song_no_loop: 
						
							; done playing 
							ld a, #0
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
	ld a, music_state.process_new_line(iy)
	or a, a
	call nz, music_process_new_line
	pop ix
	
	; clear process new line flag
	ld music_state.process_new_line(iy), #0

	; process next tic (macros etc)
	call music_process_channels_tic
	
	ret 


; reset all the channels' order and pattern pointers
; to the first order and pattern
; ix: channels
; iy: state
music_reset_channels_order_and_pattern: 

	push ix

	; get pointer to order_ptrs in hl
	ld l, music_state.order_ptrs(iy)
	ld h, music_state.order_ptrs+1(iy)

	; loop through all channels
	ld b, music_state.channel_count(iy)

	mrcoap_loop: 
		
		; get address of first order into de
		; and update it in the channel
		ld e, (hl)
		ld channel.order_table_ptr(ix), e
		inc hl
		ld d, (hl)
		ld channel.order_table_ptr+1(ix), d
		inc hl

		; using the order pointer, get the address
		; of the first pattern and update the channel
		ld a, (de)
		ld channel.pattern_ptr(ix), a
		inc de
		ld a, (de)
		ld channel.pattern_ptr+1(ix), a

		; set line_wait = 0 for channel
		ld channel.line_wait(ix), #0

		; move along to next channel
		ld de, #_sizeof_channel
		add ix, de

		djnz mrcoap_loop

	pop ix

	ret 


; move the order pointer along by one and update the pattern pointer
; to the pattern specified by the new order
; * relies on the channel struct being .order_table_ptr followed by .pattern_ptr 
; ix: channels
; iy: state
music_update_channels_order_and_pattern: 

	ld b, music_state.channel_count(iy)

    ; get channel pointer into hl
    push ix
    pop hl

    ; move it along to order_table_ptr
    ld de, #channel.order_table_ptr
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
		ld (hl), #0

		; move to next channel
		ld de, #_sizeof_channel-4
        add hl, de
		djnz mucoap_loop

	ret 
