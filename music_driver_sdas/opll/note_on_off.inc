
; banjo sound driver
; Joe Kennedy 2023

; include full opll code
.ifdef INCLUDE_OPLL

; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_note_on: 

	; restore original hl
	ex de, hl

	; send note off if there's a note currently playing and we're not playing legato
	bit CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)
	jr z, mufm_no_legato_done
	
	bit CHAN_FLAG_BIT_LEGATO, channel.flags(ix)
	jr nz, mufm_no_legato_done
	
		; send note off
		ld a, channel.subchannel(ix)
        add a, #0x20
		out (#OPLL_REG_PORT), a

		ld a, channel.freq+1(ix)
		and a, #0xf
		out (#OPLL_DATA_PORT), a

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl
		
	mufm_no_legato_done: 
	
	; select volume and instrument register to write to
	ld a, channel.subchannel(ix)
    add a, #0x30
	out (#OPLL_REG_PORT), a

	; volume
	ld a, channel.volume(ix)
	and a, #0xf
	
	; combine with shifted patch number
	or a, channel.fm_patch_shifted(ix)

	; write data
	out (#OPLL_DATA_PORT), a
	
	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)
	
	; update midi number field
	inc hl
	ld a, (hl)
	ld channel.midi_note(ix), a
	
	; look up tone data by note number
	ld de, #fm_tone_lookup
	add a, a
	add a, e
	ld e, a
	adc a, d
	sub a, e
	ld d, a

	; check whether portamento is enabled so 
	; we need to store the fnum in target frequency instead
	ld a, channel.slide_type(ix)
	cp a, #SLIDE_TYPE_PORTA
	jr z, mfmno_portamento

	mfmno_no_portamento: 

		; reset current vibrato level
		ld channel.vibrato_current(ix), #VIBRATO_CENTRE

		; store lower byte of fnum 
		ld a, (de)
		ld channel.freq(ix), a
		
		; store octave and 9th bit of f number
		inc de
		ld a, (de)
		ld channel.freq+1(ix), a

		jr mfmno_done

	mfmno_portamento: 

		; store lower byte of fnum 
		ld a, (de)
		ld channel.target_freq(ix), a
		
		; store octave and 9th bit of f number
		inc de
		ld a, (de)
		ld channel.target_freq+1(ix), a

		; check high byte of freq
		; if it's 0xff that implies no notes have played so far
		; so we write to freq too
		ld a, channel.freq+1(ix)
		cp a, #0xff
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

	; restore original hl
	ex de, hl

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; select octave + key on register
	ld a, channel.subchannel(ix)
    add a, #0x20
	out (#OPLL_REG_PORT), a
	
	; write current octave + key off
	ld a, channel.freq+1(ix)
	and a, #0xf
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	push hl
	pop hl
	push hl
	pop hl
	push hl
	pop hl

	inc hl
	jp music_process_new_line_subloop
	
; b  : current channel number
; hl : pointer to current instruction in pattern
; ix : pointer to current channel
; iy : pointer to music state
music_fm_drums_note_on: 

	; restore original hl
	ex de, hl

	; send note off if we're not playing legato and there's a note currently playing
	bit CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)
	jr z, mfmdno_legato_done
	
	bit CHAN_FLAG_BIT_LEGATO, channel.flags(ix)
	jr nz, mfmdno_legato_done
		
		; select rhythm trigger register
		ld a, #0xe
		out (#OPLL_REG_PORT), a
	
		; mask out current bit for this drum's trigger
		ld a, channel.fm_drum_trigger(ix)
        and a, #0x1f
		cpl 
		ld c, a

		; get current drum state
		ld a, (fm_drum_note_ons)
		; mask out this drum's bit and update the triggers to note-off
		and a, c
		out (#OPLL_DATA_PORT), a

		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		nop 
		nop 
		nop 
		
	mfmdno_legato_done: 

	; volume
	; select volume register for this drum
	ld a, channel.port(ix)
    add a, #0x20
	out (#OPLL_REG_PORT), a
	
	; get volume and store in c
	ld c, channel.volume(ix)
	
	; point de at current volume status for this register
	; and store in e, keep de around for later
	ld d, #>fm_drum_volumes
	ld a, #<fm_drum_volumes
    add a, channel.subchannel(ix)
    ld e, a
	adc a, d
	sub a, e
	ld d, a

    ; get volume
	ld a, (de)
	
	; mask for clearing old volume
	; mask off old volume
	and a, channel.fm_drum_volume_mask(ix)
	
	; update with the new volumes and then write back to the register
	or a, c
	out (#OPLL_DATA_PORT), a
	
	; store the new volumes there
	ld (de), a
	
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 

	; update note on status 
	set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

    ; move to midi number
    inc hl

    ; check if we should be in rhythm or fixed mode
    ld a, channel.fm_drum_trigger(ix)
    and a, #0x20
    jr nz, mfmdno_fixed_pitch

        ; set pitch register for this drum	
        ; set register for low 8 bits of f number
        ld a, channel.port(ix)
        out (#OPLL_REG_PORT), a
        
        ; update midi number field
        ld a, (hl)
        ld channel.midi_note(ix), a
        
        ; get pitch data
		ld de, #fm_tone_lookup
		add a, a
		add a, e
		ld e, a
		adc a, d
		sub a, e
		ld d, a
        
        ; low 8 bits of f number
        ld a, (de)
        out (#OPLL_DATA_PORT), a
        
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		nop 
		nop 
		nop 
		nop 

        ; set register for 9th bit of f number and octave
        ld a, channel.port(ix)
        add a, #0x10
        out (#OPLL_REG_PORT), a
        
        ; output 9th bit of f number and octave
        inc de
        ld a, (de)
        out (#OPLL_DATA_PORT), a
        
		; delay after opll data write
		push hl
		pop hl
		push hl
		pop hl
		push hl
		pop hl
		nop 
		nop 
		nop 
		nop 

    mfmdno_fixed_pitch: 

	; select rhythm control register
	ld a, #0x0e
	out (#OPLL_REG_PORT), a
	
	; get trigger bits to send to rhythm control
	ld c, channel.fm_drum_trigger(ix)
	
	; update current status of drum triggers
	ld a, (fm_drum_note_ons)
	or a, c
	or a, #0x20
	ld (fm_drum_note_ons), a
	
	; send trigger bits 
	out (#OPLL_DATA_PORT), a
	
	; delay after opll data write
	nop 
	nop 
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

	; restore original hl
	ex de, hl

	; clear note on flag
	res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; select rhythm control register
	ld a, #0x0e
	out (#OPLL_REG_PORT), a

	; get trigger bit for this drum
	ld a, channel.fm_drum_trigger(ix)
    and a, #0x1f
	
	; invert trigger byte to make a mask for this drum's bit
	cpl 
	ld c, a
	
	; mask out this drum's bit
	ld a, (fm_drum_note_ons)
	and a, c
	ld (fm_drum_note_ons), a
	
	; send trigger bits
	out (#OPLL_DATA_PORT), a

	; delay after opll data write
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 
	nop 

	inc hl
	jp music_process_new_line_subloop

; don't include full opll code
.else 

music_fm_note_on: 
music_fm_drums_note_on: 
    inc hl
music_fm_note_off: 
music_fm_drums_note_off: 
    inc hl
    jp music_process_new_line_subloop

.endif 
