
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; de : instruction pointer - needs to be exhanged with hl by the time we return
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_on_opll: 

	; standard note-on stuff
	call music_note_on

	; send note off if there's a note currently playing and we're not playing legato
	; and the channel isn't muted
	ld a, channel.flags(ix)
	and a, #CHAN_FLAG_NOTE_ON|CHAN_FLAG_LEGATO|CHAN_FLAG_MUTED
	cp a, #CHAN_FLAG_NOTE_ON
	jr nz, mnon_opll_legato_done
	
		; send note off
		ld a, channel.subchannel(ix)
        add a, #0x20
		out (#OPLL_REG_PORT), a

		ld a, channel.freq+1(ix)
		and a, #0xf
		out (#OPLL_DATA_PORT), a

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl
		
	mnon_opll_legato_done: 
	
	; get and store midi number
	inc de
	ld a, (de)
	ld channel.midi_note(ix), a
	inc de

	; multiply by 2 and use as offset into lookup table
	add a, a
	add a, #<fm_tone_lookup
	ld l, a
	adc a, #>fm_tone_lookup
	sub a, l
	ld h, a

	; get fnum value into hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; check whether portamento is enabled so 
	; we need to store the fnum in target frequency instead
	ld a, channel.slide_type(ix)
	cp a, #SLIDE_TYPE_PORTA
	jr z, mnon_opll_portamento

	mnon_opll_no_portamento: 

		; store lower byte of fnum 
		ld channel.freq(ix), l
		
		; store octave and 9th bit of f number
		ld channel.freq+1(ix), h

		jr mnon_opll_done

	mnon_opll_portamento: 

		; store lower byte of fnum 
		ld channel.target_freq(ix), l
		
		; store octave and 9th bit of f number
		ld channel.target_freq+1(ix), h

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, channel.freq+1(ix)
		cp a, #0xff
		jr z, mnon_opll_no_portamento

	mnon_opll_done: 

	; restore original hl
	ex de, hl

	jp mpnl_command_done

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_opll: 

	; restore original hl
	ex de, hl

	; standard note-off stuff
	call music_note_off

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr nz, mnoff_opll_dont_write_chip

		; select octave + key on register
		ld a, channel.subchannel(ix)
		add a, #0x20
		out (#OPLL_REG_PORT), a
		
		; write current octave + key off
		ld a, channel.freq+1(ix)
		and a, #0xf
		out (#OPLL_DATA_PORT), a
		
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl

	mnoff_opll_dont_write_chip: 

	inc hl
	jp mpnl_command_done
	
