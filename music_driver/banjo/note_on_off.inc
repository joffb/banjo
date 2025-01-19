
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; b: channel number
; ix: channel
; iy: music_state
music_note_on:

	; get and store midi number
	ld (ix + channel.midi_note), a

	; preserve hl
	ex de, hl

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

	; restore hl, fnum is in de
	ex de, hl

	; decide where to put fnum depending on portamento mode
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jr z, mnon_portamento

	mnon_no_portamento:

		; portamento off, put fnum in freq
		ld (ix + channel.freq), e
		ld (ix + channel.freq + 1), d

		jr mnon_done

	mnon_portamento:

		; portamento on, put fnum in target_freq
		ld (ix + channel.target_freq), e
		ld (ix + channel.target_freq + 1), d

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		jr z, mnon_no_portamento
			
	mnon_done:

	; mark pitch and volume as requiring update
	ld (ix + channel.events), CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE

	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	; if legato is on, we don't need to reset the macro
	bit CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)
	ret nz

		; reset macro positions
		xor a
		ld (ix + channel.volume_macro_pos), a
		ld (ix + channel.ex_macro_pos), a
		ld (ix + channel.vibrato_counter), a

		ret

; handle note-off stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_off:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	ret