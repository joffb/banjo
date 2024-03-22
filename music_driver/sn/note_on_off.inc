
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : channel pointer
; iy : pointer to music state
music_sn_note_on:

	; restore original hl
	ex de, hl

	; update note-on status 
	set CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; update volume level on chip
	ld a, (ix + channel.volume)
	ld c, (ix + channel.port)
	out (c), a

	; update midi number field
	inc hl
	ld a, (hl)
	ld (ix + channel.midi_note), a

	; multiply note number by 2 to get offset into lookup table
	ld de, sn_tone_lookup
	add a, a
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	; decide where to put fnum depending on portamento mode
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jr z, msnno_portamento

	msnno_no_portamento:

		; reset current vibrato level
		ld (ix + channel.vibrato_current), VIBRATO_CENTRE

		; portamento off, put fnum in freq
		ld a, (de)
		ld (ix + channel.freq), a

		inc de
		ld a, (de)
		ld (ix + channel.freq + 1), a

		jr msnno_done

	msnno_portamento:

		; portamento on, put fnum in target_freq
		ld a, (de)
		ld (ix + channel.target_freq), a

		inc de
		ld a, (de)
		ld (ix + channel.target_freq + 1), a

		dec de

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		jr z, msnno_no_portamento
			
	msnno_done:
	inc hl
	jp music_process_new_line_subloop

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_sn_note_off:

	; restore original hl
	ex de, hl

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	; get channel (already shifted to the correct position)
	ld a, (ix + channel.subchannel)

	; set latch = 1, type = volume, volume = 0
	or a, 0x9f
	ld c, (ix + channel.port)
	out (c), a
	
	inc hl 
	jp music_process_new_line_subloop

