
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
	and a, #0x7f
	ld channel.midi_note(ix), a

	; get flags in h
	ld h, channel.flags(ix)

	; decide where to put fnum depending on portamento mode
	bit CHAN_FLAG_BIT_SLIDE, h
	jr z, mnon_no_portamento

		; is portamento on?
		bit 2, channel.effect1(ix)
		jr z, mnon_no_portamento

			; portamento on, put fnum in target_freq
			ld channel.target_freq(ix), a

			; check high byte of freq
			; if it's >= 0x80 that implies no notes have played so far
			; so we write to freq too, by falling through below
			bit 7, channel.freq+1(ix)
			jr z, mnon_done

	mnon_no_portamento: 

		; portamento off, put fnum in freq
		ld channel.freq(ix), #0
		ld channel.freq+1(ix), a

	mnon_done: 

	; set note-on bit when the channel is not muted
	; clear note-on bit when the channel is muted
	bit CHAN_FLAG_BIT_MUTED, h
	jr nz, mno_muted

		; set note-on bit
		set CHAN_FLAG_BIT_NOTE_ON, h

		; update flags
		ld channel.flags(ix), h

		; mark pitch and volume as requiring update
		; clear release flag
		ld a, channel.events(ix)
		or a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_VOLUME_CHANGE
		and a, #~CHAN_EVENT_MACRO_RELEASE
		ld channel.events(ix), a

		; if legato is on, we don't need to reset the macro
		bit CHAN_EVENT_BIT_LEGATO, a
		ret nz

			xor a, a

			; reset vibrato position			
			ld channel.effect1(ix), a

			; reset macro positions
			inc a
			ld channel.ex_macro_pos(ix), a
			ld channel.arp_macro_pos(ix), a

			add a, #2
			ld channel.volume_macro_pos(ix), a

			ret 

	mno_muted: 

		; clear note-on bit
		res CHAN_FLAG_BIT_NOTE_ON, h
		ld channel.flags(ix), h

		; don't bother doing anything else
		ret 
