
; banjo sound driver
; Joe Kennedy 2023

banjo_mode_channel_counts:
    .db 0, 9, 4, 13, 8, 11, 0, 15

; initialize music system variables
; a: mode
banjo_init:

    ; preserve the mode in c
    ld c, a

    ; get the number of channels for this mode
    ld hl, banjo_mode_channel_counts
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; store it in b
    ld b, (hl)

	; set up array of pointers to song channels
    ; initialise pointers to channels
	ld de, song_channels
	ld hl, song_channel_ptrs

	music_init_song_channel_pointer_loop:

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

		djnz music_init_song_channel_pointer_loop

	ld a, 0xff
	ld (queue_sfx), a
	ld (queue_song), a
	
	ld a, 0
	ld (song_playing), a
	ld (sfx_playing), a
	ld (sfx_priority), a

	; assume the console is at 60hz by default
	ld a, 60
	ld (music_framerate), a

	; mute all channels
	call music_mute_all_fm
	call music_mute_all_sn
	
    ; enable/disable the FM and SN depending on the mode
    ld a, c
    and a, 0x3
    out (AUDIO_CONTROL_PORT), a

	ret

banjo_check_fm_unit_present:

    ; get current state of control port and keep in d
    in a, (AUDIO_CONTROL_PORT)
    and a, 0x3
    ld d, a

    ; 4 loops, testing values [3, 2, 1, 0]
    ld b, 4

    ; count of correct read/write matches
    ld e, 0

    detect_fm_loop:

        ; output
        ld a, b
        dec a
        ld c, a
        out (AUDIO_CONTROL_PORT), a

        ; and read it back
        in a, (AUDIO_CONTROL_PORT)
        and a, 0x3

        ; matched?
        cp a, c
        jr nz, detect_fm_no_match

            ; increase count
            inc e

        detect_fm_no_match:
        djnz detect_fm_loop

    ; output original control port state
    ld a, d
    out (AUDIO_CONTROL_PORT), a
    
    ; return 1 if the fm unit is present
    ; count = 2 implies mark 3 with fm (0, 1 will match)
    ; count = 4 implies sms with fm unit (0, 1, 2, 3 will match)
    ; counts of 1 or 3 imply no unit
    ld a, e
    cpl
    and a, 0x1
    ld (fm_unit_present), a

    ret
