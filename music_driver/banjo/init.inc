
; banjo sound driver
; Joe Kennedy 2023

; initialize music system variables
; a: number of channels
; l: chips to use
banjo_init:

	push af
	call banjo_init_system_call
	pop af

    ; store max number of channels
    ld (banjo_max_channels), a    

	; set up array of pointers to song channels
    ; initialise pointers to channels
	ld b, a
	ld de, song_channels
	ld hl, song_channel_ptrs

	bi_song_channel_pointer_loop:

		; write de to (hl)
		ld (hl), e
		inc hl
		ld (hl), d
		inc hl

		; move de to the next channel
		ld a, _sizeof_channel
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a

		djnz bi_song_channel_pointer_loop
	
    ; no songs should be playing intially
	ld a, 0
	ld (song_playing), a

	; clear first byte which has the magic byte
	ld (song_state), a

	; assume the console is at 60hz by default
	ld a, 60
	ld (music_framerate), a

    ret
