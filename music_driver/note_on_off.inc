
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : channel pointer
; iy : pointer to music state
music_sn_note_on:

	; note on
	; get volume command to send
	inc hl
	ld a, (hl)
	
	; set volume
	ld c, (ix + channel.port)
	out (c), a
	
	; store volume in channel
	and a, 0xf
	ld (ix + channel.volume), a

	; update note-on status 
	ld a, (ix + channel.flags)
	or a, CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a
	
	; update midi number field
	inc hl
	ld a, (hl)
	ld (ix + channel.midi_note), a

	; multiply note number by 2 to get offset into lookup table
	ld de, sn_tone_lookup
	sla a
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

	; clear note on flag
	ld a, (ix + channel.flags)
	and a, ~CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a

	; get channel (already shifted to the correct position)
	ld a, (ix + channel.subchannel)

	; set latch = 1, type = volume, volume = 0
	or a, 0x9f
	ld c, (ix + channel.port)
	out (c), a
	
	; store volume in channel
	and a, 0xf
	ld (ix + channel.volume), a
	
	inc hl 
	jp music_process_new_line_subloop


; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_note_on:

	; send note off if we're not playing legato and there's a note currently playing
	ld a, (ix + channel.flags)
	and a, CHAN_FLAG_NOTE_ON
	jr z, music_fm_note_on_not_legato
	
	ld a, (ix + channel.flags)
	and a, CHAN_FLAG_LEGATO
	jr nz, music_fm_note_on_not_legato
	
		; send note off
		ld a, (ix + channel.fm_base_reg)
        add a, 0x10
		out (OPLL_REG_PORT), a

		ld a, (ix + channel.freq + 1)
		out (OPLL_DATA_PORT), a
		
		nop
		nop
		nop
		nop
		nop
		nop
		
	music_fm_note_on_not_legato:
	
	; select volume and instrument register to write to
	ld a, (ix + channel.fm_base_reg)
    add a, 0x20
	out (OPLL_REG_PORT), a

	; volume
	inc hl
	ld a, (hl)
	ld c, a
	
	; combine with shifted patch number
	ld a, (ix + channel.fm_patch_shifted)
	or a, c

	; write data
	out (OPLL_DATA_PORT), a
	
	; update note on status 
	ld a, (ix + channel.flags)
	or a, CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a
	
	; update midi number field
	inc hl
	ld a, (hl)
	ld (ix + channel.midi_note), a
	
	; look up tone data by note number
	ld de, fm_tone_lookup
	sla a
	add a, e 
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	; check whether portamento is enabled so 
	; we need to store the fnum in target frequency instead
	ld a, (ix + channel.slide_type)
	cp a, SLIDE_TYPE_PORTA
	jr z, mfmno_portamento

	mfmno_no_portamento:

		; store lower byte of fnum 
		ld a, (de)
		ld (ix + channel.freq), a
		
		; store octave and 9th bit of f number
		inc de
		ld a, (de)
		ld (ix + channel.freq + 1), a

		jr mfmno_done

	mfmno_portamento:

		; store lower byte of fnum 
		ld a, (de)
		ld (ix + channel.target_freq), a
		
		; store octave and 9th bit of f number
		inc de
		ld a, (de)
		ld (ix + channel.target_freq + 1), a

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, (ix + channel.freq + 1)
		cp a, 0xff
		dec de
		jr z, mfmno_no_portamento

	mfmno_done:
	inc hl
	jp music_process_new_line_subloop

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_note_off:

	; clear note on flag
	ld a, (ix + channel.flags)
	and a, ~CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a

	; select octave + key on register
	ld a, (ix + channel.fm_base_reg)
    add a, 0x10
	out (OPLL_REG_PORT), a
	
	; write current octave + key off
	ld a, (ix + channel.freq + 1)
	out (OPLL_DATA_PORT), a

	nop
	nop
	nop
	nop
	nop
	nop

	inc hl
	jp music_process_new_line_subloop
	
; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_drums_note_on:

	; send note off if we're not playing legato and there's a note currently playing
	ld a, (ix + channel.flags)
	and a, CHAN_FLAG_NOTE_ON
	jr z, mfmdno_legato_done
	
	ld a, (ix + channel.flags)
	and a, CHAN_FLAG_LEGATO
	jr nz, mfmdno_legato_done
		
		; select rhythm trigger register
		ld a, 0xe
		out (OPLL_REG_PORT), a
	
		; mask out current bit for this drum's trigger
		ld a, (ix + channel.fm_drum_trigger)
        and a, 0x1f
		cpl
		ld c, a

		; get current drum state
		ld a, (fm_drum_note_ons)
		; mask out this drum's bit and update the triggers to note-off
		and a, c
		out (OPLL_DATA_PORT), a

		nop
		nop
		nop
		nop
		nop
		nop
		
	mfmdno_legato_done:

	; volume
	; select volume register for this drum
	ld a, (ix + channel.fm_base_reg)
    add a, 0x20
	out (OPLL_REG_PORT), a
	
	; get volume and store in c
	inc hl
	ld c, (hl)
	
	; point de at current volume status for this register
	; and store in e, keep de around for later
	ld d, >fm_drum_volumes
	ld a, <fm_drum_volumes
    add a, (ix + channel.subchannel)
    ld e, a
	adc a, d
	sub a, e
	ld d, a

    ; get volume
	ld a, (de)
	
	; mask for clearing old volume
	; mask off old volume
	and a, (ix + channel.fm_drum_volume_mask)
	
	; update with the new volumes and then write back to the register
	or a, c
	out (OPLL_DATA_PORT), a
	
	; store the new volumes there
	ld (de), a
	
	nop
	nop
	nop
	nop
	nop
	nop
	
	; update note on status 
	ld a, (ix + channel.flags)
	or a, CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a

    ; move to midi number
    inc hl

    ; check if we should be in rhythm or fixed mode
    ld a, (ix + channel.fm_drum_trigger)
    and a, 0x20
    jr nz, mfmdno_fixed_pitch

        ; set pitch register for this drum	
        ; set register for low 8 bits of f number
        ld a, (ix + channel.fm_base_reg)
        out (OPLL_REG_PORT), a
        
        ; update midi number field
        ld a, (hl)
        ld (ix + channel.midi_note), a
        
        ; get pitch data
		ld de, fm_tone_lookup
		sla a
		add a, e 
		ld e, a
		adc a, d
		sub a, e
		ld d, a
        
        ; low 8 bits of f number
        ld a, (de)
        out (OPLL_DATA_PORT), a
        
        nop
        nop
        nop
        nop
        nop
        nop
        
        ; set register for 9th bit of f number and octave
        ld a, (ix + channel.fm_base_reg)
        add a, 0x10
        out (OPLL_REG_PORT), a
        
        ; output 9th bit of f number and octave
        inc de
        ld a, (de)
        out (OPLL_DATA_PORT), a
        
        nop
        nop
        nop
        nop
        nop
        nop
	
    mfmdno_fixed_pitch:

	; select rhythm control register
	ld a, 0x0e
	out (OPLL_REG_PORT), a
	
	; get trigger bits to send to rhythm control
	ld c, (ix + channel.fm_drum_trigger)
	
	; update current status of drum triggers
	ld a, (fm_drum_note_ons)
	or a, c
	or a, 0x20
	ld (fm_drum_note_ons), a
	
	; send trigger bits 
	out (OPLL_DATA_PORT), a
	
	nop
	nop
	nop
	nop
	nop
	nop
	
	inc hl
	jp music_process_new_line_subloop

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_drums_note_off:

	; clear note on flag
	ld a, (ix + channel.flags)
	and a, ~CHAN_FLAG_NOTE_ON
	ld (ix + channel.flags), a

	; select rhythm control register
	ld a, 0x0e
	out (OPLL_REG_PORT), a

	; get trigger bit for this drum
	ld a, (ix + channel.fm_drum_trigger)
    and a, 0x1f
	
	; invert trigger byte to make a mask for this drum's bit
	cpl
	ld c, a	
	
	; mask out this drum's bit
	ld a, (fm_drum_note_ons)
	and a, c
	ld (fm_drum_note_ons), a
	
	; send trigger bits
	out (OPLL_DATA_PORT), a

	nop
	nop
	nop
	nop
	nop
	nop

	inc hl
	jp music_process_new_line_subloop
