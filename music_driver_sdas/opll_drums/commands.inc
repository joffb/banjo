
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; iy: music_state
; ix: channel we're updating
mpnl_opll_drum: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

	; preserve bc
	push bc

    ; get instrument number in a
    ld channel.instrument_num(ix), a

    ; .assert  _sizeof_instrument==12; ensure that an instrument is the size we think it is!

	; get instrument pointer by doing instrument_num * 12
	ld l, a
	ld h, #0
	
	; * 4
	add hl, hl
	add hl, hl

	; keep * 4 for later
	ld b, h
	ld c, l
	
	; * 8
	add hl, hl
	
	; * 12
	add hl, bc

	; offset into instrument data
	ld c, music_state.instrument_ptrs(iy)
	ld b, music_state.instrument_ptrs+1(iy)
	add hl, bc

	; moved along to fm patch data
	ld bc, #instrument.fm_preset
	add hl, bc

    ; is this a fixed-frequency patch?
    ld a, (hl)
    inc a
    jr z, music_fm_drum_change_fixed_freq

        ; not fixed freq
        xor a, a
        ld (opll_drum_fixed_pitch_mode), a
        jr music_fm_drum_change_done

    music_fm_drum_change_fixed_freq: 

		; enable fixed pitch mode on drum trigger
        ld a, #1
        ld (opll_drum_fixed_pitch_mode), a

		; get patch data address into hl
		inc hl
		ld a, (hl)
		inc hl
		ld h, (hl)
		ld l, a

		; prepare b & c registers for use with outi
		ld b, #0
		ld c, #OPLL_DATA_PORT

		; kick?
		ld a, #0x16
		out (#OPLL_REG_PORT), a
		outi 

		call _banjo_opll_write_delay

		ld a, #0x26
		out (#OPLL_REG_PORT), a
		outi 

		call _banjo_opll_write_delay

		; snare?
		ld a, #0x17
		out (#OPLL_REG_PORT), a
		outi 

		call _banjo_opll_write_delay

		ld a, #0x27
		out (#OPLL_REG_PORT), a
		outi 
		
		call _banjo_opll_write_delay
		
		; tom?
		ld a, #0x18
		out (#OPLL_REG_PORT), a
		outi 
		
		call _banjo_opll_write_delay

		ld a, #0x28
		out (#OPLL_REG_PORT), a
		outi 

	music_fm_drum_change_done: 

	; restore bc
	pop bc

	jp mpnl_command_done
