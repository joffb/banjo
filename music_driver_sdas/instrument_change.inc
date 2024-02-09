
; banjo sound driver
; Joe Kennedy 2023

; change the instrument on this channel
; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_instrument_change: 

	; get current instrument
	ld a, channel.instrument_num(ix)
	ld c, a

	; get new instrument
	inc hl
	ld a, (hl)
	
	; don't do anything if the instrument number is the same
	cp a, c
	jr z, music_instrument_change_done
	
	; store the new instrument number in the channel
	ld channel.instrument_num(ix), a
	
	; get pointer to instrument address table in de
	ld e, music_state.instrument_ptrs(iy)
	ld d, music_state.instrument_ptrs+1(iy)
	
	; offset to get instrument pointer by adding instrument_num * 2 to de
    add a, a
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

    ; get pointer to instrument into de
	ld a, (de)
	ld c, a
	inc de
	ld a, (de)
	ld d, a
	ld e, c
	
	; volume macro
	; copy over macro info
	ld a, (de)
	ld channel.volume_macro_len(ix), a
	inc de
	ld a, (de)
	ld channel.volume_macro_mode(ix), a
	inc de
	ld a, (de)
	ld channel.volume_macro_loop(ix), a
	inc de
	ld a, (de)
	ld channel.volume_macro_ptr(ix), a
	inc de
	ld a, (de)
    ld channel.volume_macro_ptr+1(ix), a
	
	; set initial macro position
	ld a, #0
	ld channel.volume_macro_pos(ix), a
	
	; check if this is a normal fm channel, skip ahead if it's not
	ld a, channel.type(ix)
	cp a, #CHAN_OPLL
	jr nz, music_instrument_change_done
		
		; it is an fm instrument, store the patch number shifted left by 4
		inc de
		ld a, (de)
		ld channel.fm_patch_shifted(ix), a
		
		; if it's patch 0, update the fm registers with the custom patch data
		; otherwise we're done
		or a, a
		jr nz, music_instrument_change_done
		
			; load custom patch
			inc de

			; start register number
			ld c, #0x00
			
			music_instrument_change_fm_patch: 
			
				ld a, c
				out (#OPLL_REG_PORT), a
				
				ld a, (de)
				out (#OPLL_DATA_PORT), a
				
				nop 
				nop 
				nop 
				nop 
				nop 
				nop 
				
				; next register and next patch data
				inc de
				inc c
				
				; reached the last register?
				ld a, #0x08
				cp a, c
				jr nz, music_instrument_change_fm_patch
		
	music_instrument_change_done: 
	
	inc hl
	jp music_process_new_line_subloop

; hl: music instruction pointer
music_fm_drum_change: 

    ; get instrument number in a
    inc hl
    ld a, (hl)
    ld channel.instrument_num(ix), a

    ; preserve hl in de
    ex de, hl

    ; get pointer to pointer to instrument in hl
	ld l, music_state.instrument_ptrs(iy)
	ld h, music_state.instrument_ptrs+1(iy)

    ; double it to get offset into instrument pointers
    add a, a

    ; apply offset
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; aiming to get instrument pointer into hl
    ld a, (hl)
    inc hl
    ld h, (hl)

    ; move hl along to fm patch data
    add a, #5
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; restore hl from de
    ; pointer to fm data now in de
    ex de, hl

    ; is this a fixed-frequency patch?
    ld a, (de)
    cp a, #0xff
    jr z, music_fm_drum_change_fixed_freq

        ; not fixed freq
        ld a, channel.fm_drum_trigger(ix)
        and a, #0x1f
        ld channel.fm_drum_trigger(ix), a
        jr music_fm_drum_change_done

    music_fm_drum_change_fixed_freq: 

    ; enable fixed pitch mode on drum trigger
    ld a, channel.fm_drum_trigger(ix)
    or a, #0x20
    ld channel.fm_drum_trigger(ix), a

    ; fixed pitch fnums and blocks
    inc de

	; kick?
	ld a, #0x16
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a

    inc de

    nop 
    nop 
    nop 
    nop 
    nop 
    nop 

	ld a, #0x26
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a

    inc de

    nop 
    nop 
    nop 
    nop 
    nop 
    nop 

	; snare?
	ld a, #0x17
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a

    inc de

    nop 
    nop 
    nop 
    nop 
    nop 
    nop 

	ld a, #0x27
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a

    inc de
	
    nop 
    nop 
    nop 
    nop 
    nop 
    nop 

	; tom?
	ld a, #0x18
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a

    inc de
	
    nop 
    nop 
    nop 
    nop 
    nop 
    nop 

	ld a, #0x28
	out (#OPLL_REG_PORT), a
	ld a, (de)
	out (#OPLL_DATA_PORT), a


    music_fm_drum_change_done: 
	inc hl
	jp music_process_new_line_subloop
