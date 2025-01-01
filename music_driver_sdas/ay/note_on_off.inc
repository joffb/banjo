
; banjo sound driver
; Joe Kennedy 2024

; b  : current channel number
; de : instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel pointer
; iy : pointer to music state
mpnl_note_on_ay: 

	; standard note-on stuff
	call music_note_on

	; get and store midi number
	inc de
	ld a, (de)
	ld channel.midi_note(ix), a
	inc de

	; multiply by 2 and use as offset into lookup table
	add a, a
	add a, #<ay_tone_lookup
	ld l, a
	adc a, #>ay_tone_lookup
	sub a, l
	ld h, a

	; get fnum value into hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; decide where to put fnum depending on portamento mode
	ld a, channel.slide_type(ix)
	cp a, #SLIDE_TYPE_PORTA
	jr z, mnon_ay_portamento

	mnon_ay_no_portamento: 

		; portamento off, put fnum in freq
		ld channel.freq(ix), l
		ld channel.freq+1(ix), h

		jr mnon_ay_done

	mnon_ay_portamento: 

		; portamento on, put fnum in target_freq
		ld channel.target_freq(ix), l
		ld channel.target_freq+1(ix), h

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, channel.freq+1(ix)
		cp a, #0xff
		jr z, mnon_ay_no_portamento
			
	mnon_ay_done: 

	; restore original hl
	ex de, hl

	jp mpnl_command_done

; ix : pointer to current channel
; iy : pointer to music state
mpnl_note_off_ay: 

	; standard note-off stuff
	call music_note_off

	; restore original hl
	ex de, hl

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jr nz, mnoff_sn_dont_write_chip

		; get AY_REG_WRITE
		ld c, channel.port(ix)

		; update volume level on chip
		; select volume register
		ld a, channel.subchannel(ix)
		add a, #8
		out (c), a

		; move to to AY_DATA_WRITE
		inc c

		; write 0 to register
		; setting note volume to 0 and switching off envelope
		ld a, #0
		out (c), a

	mnoff_sn_dont_write_chip: 
	
	inc hl

	jp mpnl_command_done
