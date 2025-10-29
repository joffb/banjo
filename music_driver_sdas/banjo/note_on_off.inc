
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix: channel
; iy: music_state
music_note_on: 

	; get and store midi number
	and a, #0x7f
	ld channel.midi_note(ix), a

	; note in l
	ld l, a

	; flags in h
	ld h, channel.flags(ix)
	
	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, h
	ld channel.flags(ix), h

	; decide where to put fnum depending on portamento mode
	bit CHAN_FLAG_BIT_SLIDE, h
	jr z, mnon_no_portamento

		; portamento on?
		ld a, channel.effect1(ix)
		cp a, #SLIDE_TYPE_PORTA
		jr z, mnon_portamento

	mnon_no_portamento: 

		; portamento off, put fnum in freq
		ld channel.freq(ix), #0
		ld channel.freq+1(ix), l

		jr mnon_done

	mnon_portamento: 

		; portamento on, put fnum in target_freq
		ld channel.target_freq(ix), l

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, channel.freq+1(ix)
		cp a, #0xff
		jr z, mnon_no_portamento
			
	mnon_done: 

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
		bit CHAN_FLAG_BIT_VIBRATO, h
		jr z, mnon_no_vibrato
		
			ld channel.effect1(ix), a

		mnon_no_vibrato: 

		; reset macro positions
		inc a
		ld channel.ex_macro_pos(ix), a
		ld channel.arp_macro_pos(ix), a

		add a, #2
		ld channel.volume_macro_pos(ix), a

		ret 
