
; banjo sound driver
; Joe Kennedy 2023

.ifndef QUEUES_OFF

; a: song number to queue
banjo_queue_song:

    ld (queue_song), a

	; clear loop mode modifier variable
	ld a, 0xff
	ld (queue_song_loop), a

    ret

; a: sfx number to queue
banjo_queue_sfx:

    ld (queue_sfx), a

	; clear loop mode modifier variable
	ld a, 0xff
	ld (queue_sfx_loop), a

    ret

; a: loop mode for queued sfx
banjo_queue_song_loop_mode:

	ld (queue_song_loop), a

	ret

; a: loop mode for queued sfx
banjo_queue_sfx_loop_mode:

	ld (queue_sfx_loop), a

	ret

music_handle_sfx_queue:
	
	ld a, (queue_sfx)
	
	; get address of queued sfx in table
	; (multiply a by 4 and add to hl)
	ld hl, (sfx_table_ptr)
	add a, a
	add a, a
	add a, l
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; check priority of current sfx vs new priority
	; don't play if if the priority is lower
	ld a, (sfx_priority)
	ld b, a
	ld a, (hl)
	cp a, b
	jr c, mhsfq_done
		
		; update priority variable
		ld a, b
		ld (sfx_priority), a
		
		; get bank and change slot 2 bank to this bank
		inc hl
		ld a, (hl)
		ld (SLOT_2_BANK_CHANGE), a

		; keep it in c
		ld c, a
		
		; get address of sfx into hl
		inc hl
		ld a, (hl)
		inc hl
		ld h, (hl)
		ld l, a

		; check if it's a valid song from the header
		ld a, (hl)
		cp a, BANJO_MAGIC_BYTE
		jr nz, mhsfq_done
			
			ld a, (queue_sfx_loop)
			ld b, a
			ld ix, sfx_channel
			ld iy, sfx_state
			call music_play

			; sfx is now playing
			ld a, 1
			ld (sfx_playing), a
			
	mhsfq_done:
	
	ld a, 0xff
	ld (queue_sfx), a

	ret

music_handle_song_queue:

	; check whether a song has been queued
	ld a, (queue_song)

	; add offset to song table
	ld hl, (song_table_ptr)
	add a, a
	add a, a
	add a, l
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; get bank and change slot 2 bank to this bank
	inc hl
	ld a, (hl)
	ld (SLOT_2_BANK_CHANGE), a

	; keep it in c
	ld c, a
	
	; get address of song into hl
	inc hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; check if it's a valid song from the header
	ld a, (hl)
	cp a, BANJO_MAGIC_BYTE
	jr nz, mhsoq_done

		ld a, (queue_song_loop)
		ld b, a
		ld ix, song_channels
		ld iy, song_state
		call music_play
		
		; song is now playing
		ld a, 1
		ld (song_playing), a

	mhsoq_done:
	; clear song queue
	ld a, 0xff
	ld (queue_song), a
	
	ret

.else

; a: song number to queue
banjo_queue_song:
banjo_queue_sfx:
banjo_queue_song_loop_mode:
banjo_queue_sfx_loop_mode:
music_handle_sfx_queue:
music_handle_song_queue:
	ret

.endif