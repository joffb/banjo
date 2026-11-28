
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; a: midi number
; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix: channel
; iy: music_state
music_note_on:

	; decide where to put fnum depending on portamento mode
	bit CHAN_EVENT_BIT_SLIDE, b
	jr z, mnon_no_portamento

		; is portamento on?
		bit 2, (ix + channel.effect3)
		jr z, mnon_no_portamento

			; portamento on, put fnum in target_freq
			ld (ix + channel.target_freq), a

			; check high byte of freq
			; if it's >= 0x80 that implies no notes have played so far
			; so we write to freq too, by falling through below
			bit 7, (ix + channel.freq + 1)
			jr z, mnon_done

	mnon_no_portamento:

		; portamento off, put fnum in freq
		ld (ix + channel.freq), 0
		ld (ix + channel.freq + 1), a

	mnon_done:

	; set note-on bit when the channel is not muted
	; clear note-on bit when the channel is muted
	bit CHAN_FLAG_BIT_MUTED, c
	jr nz, mno_muted

		; set note-on bit
		set CHAN_FLAG_BIT_NOTE_ON, c

		; clear release bit
		res CHAN_FLAG_BIT_MACRO_RELEASE, c

		; mark pitch and volume as requiring update
		; clear release flag
		ld a, b
		or a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE
		ld b, a

		; if legato is on, we don't need to reset the macro
		bit CHAN_FLAG_BIT_LEGATO, c
		ret nz

			xor a, a

			; reset vibrato/arp effect position
			; reset macro positions
			ld (ix + channel.effect1), a
			ld (ix + channel.volume_macro_pos), a
			ld (ix + channel.arp_macro_pos), a
			ld (ix + channel.ex_macro_pos), a

			ret

	mno_muted:

		; clear note-on bit
		res CHAN_FLAG_BIT_NOTE_ON, c

		; don't bother doing anything else
		ret