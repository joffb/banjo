
; banjo sound driver
; Joe Kennedy 2023

; initialize music system variables
; a: channel count
_banjo_init: 

	; set up array of pointers to song channels
	ld de, #_song_channels
	ld hl, #_song_channel_ptrs
    ld b, a

	music_init_song_channel_pointer_loop: 

		; write de to (hl)
		ld (hl), e
		inc hl
		ld (hl), d
		inc hl

		; move de to the next channel
		ld a, #_sizeof_channel
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a

		djnz music_init_song_channel_pointer_loop

	ld a, #0xff
	ld (queue_sfx), a
	ld (queue_song), a
	
	ld a, #0
	ld (song_playing), a
	ld (sfx_playing), a
	ld (sfx_priority), a

	; assume the console is at 60hz by default
	ld a, #60
	ld (music_framerate), a

    ; see whether the fm chip is present
    call check_fm_unit_present

	; mute all channels
	call music_mute_all_fm
	call music_mute_all_sn
	
    ; unmute SN and FM
    ld a, #0x3
    out (#AUDIO_CONTROL_PORT), a


	ret 

; returns 1 if fm unit present, 0 otherwise
_banjo_fm_unit_present: 
    ld a, (fm_unit_present)
    ret 


check_fm_unit_present: 

    ; get current state of control port and keep in d
    in a, (#AUDIO_CONTROL_PORT)
    and a, #0x3
    ld d, a

    ; 4 loops, testing values [3, 2, 1, 0]
    ld b, #4

    ; count of correct read/write matches
    ld e, #0

    detect_fm_loop: 

        ; output
        ld a, b
        dec a
        ld c, a
        out (#AUDIO_CONTROL_PORT), a

        ; and read it back
        in a, (#AUDIO_CONTROL_PORT)
        and a, #0x3

        ; matched?
        cp a, c
        jr nz, detect_fm_no_match

            ; increase count
            inc e

        detect_fm_no_match: 
        djnz detect_fm_loop

    ; output original control port state
    ld a, d
    out (#AUDIO_CONTROL_PORT), a
    
    ; return 1 if the fm unit is present
    ; count = 2 implies mark 3 with fm (0, 1 will match)
    ; count = 4 implies sms with fm unit (0, 1, 2, 3 will match)
    ; counts of 1 or 3 imply no unit
    ld a, e
    cpl 
    and a, #0x1
    ld (fm_unit_present), a

    ret 
