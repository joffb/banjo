
; banjo sound driver
; Joe Kennedy 2023

; initialize music system variables
; a: number of channels
; l: chips to use
_banjo_init: 

	push af
	call _banjo_init_system_call
	pop af

    ; store max number of channels
    ld (_banjo_max_channels), a

    ; no songs should be playing intially
	xor a, a
	ld (song_playing), a

	; clear first byte which has the magic byte
	ld (_song_state), a

	; assume the console is at 60hz by default
	ld a, #60
	ld (music_framerate), a

    ret 
