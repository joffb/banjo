
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix: channel
; iy: music_state
music_note_on:

	; restrict to 0-127 and save in channel
	and a, 0x7f
	ld (ix + channel.midi_note), a

	; note in l
	ld l, a

	; flags in h
	ld h, (ix + channel.flags)
	
	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, h
	ld (ix + channel.flags), h

	; decide where to put fnum depending on portamento mode
	bit CHAN_FLAG_BIT_SLIDE, h
	jr z, mnon_no_portamento

		; portamento on?
		ld a, (ix + channel.effect1)
		cp a, SLIDE_TYPE_PORTA
		jr z, mnon_portamento

	mnon_no_portamento:

		; portamento off, put fnum in freq
		ld (ix + channel.freq), 0
		ld (ix + channel.freq + 1), l

		jr mnon_done

	mnon_portamento:

		; portamento on, put fnum in target_freq
		ld (ix + channel.target_freq), l

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		jr z, mnon_no_portamento
			
	mnon_done:

	; mark pitch and volume as requiring update
	; clear release flag
	ld a, (ix + channel.events)
	or a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE
	and a, ~CHAN_EVENT_MACRO_RELEASE
	ld (ix + channel.events), a

	; if legato is on, we don't need to reset the macro
	bit CHAN_EVENT_BIT_LEGATO, a
	ret nz

		xor a, a

		; reset vibrato position
		bit CHAN_FLAG_BIT_VIBRATO, h
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