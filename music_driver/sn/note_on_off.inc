
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; de : instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel
; iy : music state
mpnl_note_on_sn:

	; standard note-on stuff
	call music_note_on

	; get and store midi number
	inc de
	ld a, (de)
	ld (ix + channel.midi_note), a
	inc de

	; multiply by 2 and use as offset into lookup table
	add a, a
	add a, <sn_tone_lookup
	ld l, a
	adc a, >sn_tone_lookup
	sub a, l
	ld h, a

	; get fnum value into hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; decide where to put fnum depending on portamento mode
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jr z, mnon_sn_portamento

	mnon_sn_no_portamento:

		; portamento off, put fnum in freq
		ld (ix + channel.freq), l
		ld (ix + channel.freq + 1), h

		jr mnon_sn_done

	mnon_sn_portamento:

		; portamento on, put fnum in target_freq
		ld (ix + channel.target_freq), l
		ld (ix + channel.target_freq + 1), h

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		jr z, mnon_sn_no_portamento
			
	mnon_sn_done:
	
	; restore original hl
	ex de, hl

	jp mpnl_command_done

; de: instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel
; iy : music state
mpnl_note_off_sn:

	; standard note-off stuff
	call music_note_off

	; restore original hl
	ex de, hl

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_sn_dont_write_chip

		; get channel (already shifted to the correct position)
		ld a, (ix + channel.subchannel)

		; set latch = 1, type = volume, volume = 0
		or a, 0x9f
		ld c, (ix + channel.port)
		out (c), a

        ; ALF TEST
        ; additional write delay
		.ifdef BANJO_ALF
			call banjo_sn_alf_wait
		.endif

	mnoff_sn_dont_write_chip:
	
	inc hl
	jp mpnl_command_done
