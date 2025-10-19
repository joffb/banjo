
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; b: channel number
; ix: channel
; iy: music_state
music_note_on: 

	; get and store midi number
	and a, #0x7f
	ld channel.midi_note(ix), a

	; multiply by 2 and use as offset into lookup table
	add a, a
	add a, l
	ld l, a
	adc a, h
	sub a, l
	ld h, a

	; get fnum into hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; decide where to put fnum depending on portamento mode
	ld a, channel.slide_type(ix)
	cp a, #SLIDE_TYPE_PORTA
	jr z, mnon_portamento

	mnon_no_portamento: 

		; portamento off, put fnum in freq
		ld channel.freq(ix), l
		ld channel.freq+1(ix), h

		jr mnon_done

	mnon_portamento: 

		; portamento on, put fnum in target_freq
		ld channel.target_freq(ix), l
		ld channel.target_freq+1(ix), h

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, channel.freq+1(ix)
		cp a, #0xff
		jr z, mnon_no_portamento
			
	mnon_done: 

	; mark pitch and volume as requiring update
	; clear release flag
	ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_VOLUME_CHANGE
	or a, channel.events(ix)
	res CHAN_EVENT_BIT_MACRO_RELEASE, a
	ld channel.events(ix), a

	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; if legato is on, we don't need to reset the macro
	bit CHAN_FLAG_BIT_LEGATO, channel.flags(ix)
	ret nz

		xor a
		ld channel.vibrato_counter(ix), a

		; reset macro positions
		ld a, #3
		ld channel.volume_macro_pos(ix), a
		ld channel.ex_macro_pos(ix), a

		ret 

; handle note-off stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_off: 

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	ret 
