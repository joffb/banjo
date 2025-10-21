
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; b: channel number
; ix: channel
; iy: music_state
music_note_on:

	; get and store midi number
	and a, 0x7f
	ld (ix + channel.midi_note), a

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
	bit CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)
	jr z, mnon_no_portamento

		ld a, (ix + channel.effect1)
		cp a, SLIDE_TYPE_PORTA
		jr z, mnon_portamento

	mnon_no_portamento:

		; portamento off, put fnum in freq
		ld (ix + channel.freq), l
		ld (ix + channel.freq + 1), h

		jr mnon_done

	mnon_portamento:

		; portamento on, put fnum in target_freq
		ld (ix + channel.target_freq), l
		ld (ix + channel.target_freq + 1), h

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		jr z, mnon_no_portamento
			
	mnon_done:

	; mark pitch and volume as requiring update
	; clear release flag
	ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE
	or a, (ix + channel.events)
	res CHAN_EVENT_BIT_MACRO_RELEASE, a
	ld (ix + channel.events), a

	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	; if legato is on, we don't need to reset the macro
	bit CHAN_EVENT_BIT_LEGATO, (ix + channel.events)
	ret nz

		xor a, a

		; reset vibrato
		bit CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)
		jr z, mnon_no_vibrato
		
			ld (ix + channel.effect1), a

		mnon_no_vibrato:

		; reset macro positions
		inc a
		ld (ix + channel.ex_macro_pos), a
		ld (ix + channel.arp_macro_pos), a

		add a, 2
		ld (ix + channel.volume_macro_pos), a

		ret

; handle note-off stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_off:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	ret