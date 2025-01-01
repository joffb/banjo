
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line
	
; iy: music_state
; ix: channel we're updating
mpnl_opll_drum:

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

    ; at this point de is still the instruction/command pointer

    ; get instrument number in a
    inc de
    ld a, (de)
    ld (ix + channel.instrument_num), a

    ; double instrument number to get offset into instrument pointers
    add a, a

    ; apply offset
    add a, (iy + music_state.instrument_ptrs)
    ld l, a
    adc a, (iy + music_state.instrument_ptrs + 1)
    sub a, l
    ld h, a

    ; get instrument pointer into hl
	; moved along to fm patch data
    ld a, (hl)
	add a, instrument.fm_preset
	ld c, a
    inc hl
    adc a, (hl)
	sub a, c
	ld h, a
	ld l, c	

    ; is this a fixed-frequency patch?
    ld a, (hl)
    cp a, 0xff
    jr z, music_fm_drum_change_fixed_freq

        ; not fixed freq
        ld a, 0
        ld (opll_drum_fixed_pitch_mode), a
        jr music_fm_drum_change_done

    music_fm_drum_change_fixed_freq:

		; enable fixed pitch mode on drum trigger
        ld a, 1
        ld (opll_drum_fixed_pitch_mode), a

		; get patch data address into hl
		inc hl
		ld a, (hl)
		inc hl
		ld h, (hl)
		ld l, a

		; kick?
		ld a, 0x16
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

		inc hl

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl

		ld a, 0x26
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

		inc hl

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl

		; snare?
		ld a, 0x17
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

		inc hl

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl

		ld a, 0x27
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

		inc hl
		
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl
		
		; tom?
		ld a, 0x18
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

		inc hl
		
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl 

		ld a, 0x28
		out (OPLL_REG_PORT), a
		ld a, (hl)
		out (OPLL_DATA_PORT), a

    music_fm_drum_change_done:
	ex de, hl
	inc hl
	jp mpnl_command_done