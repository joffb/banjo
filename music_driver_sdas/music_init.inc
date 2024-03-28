
; banjo sound driver
; Joe Kennedy 2023

; see MODE_* definitions for indexes
; e.g. MODE_FM == 1 == 9 channels
banjo_mode_channel_counts: 
    .db 0, 9,4,13,8,11,0,15

; initialize music system variables
; a: mode
_banjo_init: 

    ; preserve the mode in variable and in c 
    ld (_banjo_mode), a
    ld c, a

    ; get the number of channels for this mode
    ld hl, #banjo_mode_channel_counts
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; store it in b
    ld b, (hl)

	; set up array of pointers to song channels
    ; initialise pointers to channels
	ld de, #_song_channels
	ld hl, #song_channel_ptrs

	bi_song_channel_pointer_loop: 

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

		djnz bi_song_channel_pointer_loop

    ; initialise queues
	ld a, #0xff
	ld (queue_sfx), a
    ld (queue_sfx_loop), a
	ld (queue_song), a
    ld (queue_song_loop), a
	
    ; no songs or sfx should be playing intially
	ld a, #0
	ld (song_playing), a
	ld (sfx_playing), a
	ld (sfx_priority), a

	; assume the console is at 60hz by default
	ld a, #60
	ld (music_framerate), a

	; mute channels depending on the mode

    ; mode bit 0 == 1 means fm
    bit 0, c
	call nz, music_mute_all_fm

    ; mode bit 1 == 1 means sn
    bit 1, c
	call nz, music_mute_all_sn
	
    ; enable/disable the FM and SN depending on the mode
    ld a, c
    and a, #0x3
    out (#AUDIO_CONTROL_PORT), a

	ret 

_banjo_set_song_table: 
    ld (song_table_ptr), hl
    ret 

_banjo_set_sfx_table: 
    ld (sfx_table_ptr), hl
    ret 

_banjo_check_hardware: 

    ; check for fm unit

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
    ld (_banjo_fm_unit_present), a

    ; check for game gear mode

    ; read from i/o port 0
    ; if there's anything in the lowest 6 bits, we're not in game gear mode
    ; on sms 1 the port should read 0x78 which is the last byte of the  "in a, (c)" mpnl_instruction_check_done
    ; on sms 2 the port should read 0xff
    ; on game gear bits 0-5 should be empty and bits 6 and 7 depend on region and whether the start button is being held down

    ld c, #GAME_GEAR_PORT_0
    in a, (c)

    ; isolate lower bits
    and a, #0x3f
    jr nz, bch_not_game_gear

        ld a, #1
        jr bch_done

    bch_not_game_gear: 

        ld a, #0

    bch_done: 
    ld (_banjo_game_gear_mode), a


    ; check for Sega System E by seeing if there's 16kb of RAM 
    ; rather than 8kb with 0xe000 being a mirror of 0xc000

    ld de, #_banjo_system_e+8192
    ld hl, #_banjo_system_e

    ; number of values to write for test
    ld b, #8

    ; number of matched mirror read/writes
    ld c, #0

    bch_check_system_e: 

        ; write b to (banjo_system_e) variable
        ld (hl), b

        ; read (banjo_system_e variable  + 8192) and compare it
        ld a, (de)
        cp a, (hl)
        jr z, bch_not_mirrored

            ; (banjo_system_e) write != (banjo_system_e variable  + 8192) read
            inc c

        bch_not_mirrored: 
        djnz bch_check_system_e

    ; c == 0 if all (banjo_system_e variable  + 8192) reads matched the (banjo_system_e) writes
    ld a, c
    or a, a
    jr z, bch_check_system_e_done

        ld a, #1

    bch_check_system_e_done: 

    ; update variable with flag
    ld (_banjo_system_e), a

    ret 
