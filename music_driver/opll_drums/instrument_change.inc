
; banjo sound driver
; Joe Kennedy 2024


; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; iy: music_state
; ix: channel we're updating
banjo_instrument_change_opll_drums:

	; get patch data pointer into hl
	ex de, hl

    ; is this a fixed-frequency patch?
    ld a, (hl)
    inc a
    jr z, music_fm_drum_change_fixed_freq

        ; not fixed freq
        xor a, a
        ld (opll_drum_fixed_pitch_mode), a

        jp banjo_instrument_change_done

    music_fm_drum_change_fixed_freq:

		; enable fixed pitch mode on drum trigger
        ld a, 1
        ld (opll_drum_fixed_pitch_mode), a

		; move to drum patch data
		inc hl

		; prepare b & c registers for use with outi
		ld b, 0
		ld c, OPLL_DATA_PORT

		; kick?
		ld a, 0x16
		out (OPLL_REG_PORT), a
		outi

		call banjo_opll_write_delay

		ld a, 0x26
		out (OPLL_REG_PORT), a
		outi

		call banjo_opll_write_delay

		; snare?
		ld a, 0x17
		out (OPLL_REG_PORT), a
		outi

		call banjo_opll_write_delay

		ld a, 0x27
		out (OPLL_REG_PORT), a
		outi
		
		call banjo_opll_write_delay
		
		; tom?
		ld a, 0x18
		out (OPLL_REG_PORT), a
		outi
		
		call banjo_opll_write_delay

		ld a, 0x28
		out (OPLL_REG_PORT), a
		outi

		jp banjo_instrument_change_done