
; banjo sound driver
; Joe Kennedy 2023

; get fnum for note into de
; hl: tone lookup table address
; ix: channel
; iy: state
music_calc_fnum: 
	
	; check if we have a pitch slide going as
	; it takes precedence over arpeggios
	bit CHAN_FLAG_BIT_SLIDE, channel.flags(ix)
	jr nz, mcf_no_arp

		; check if we have an arpeggio running
		bit CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)
		jr z, mcf_no_arp

		; check arpeggio pos
		ld a, channel.arpeggio_pos(ix)
		or a, a
		jr z, mcf_arp_pos_0

			; isolate lower nibble
			; the arpeggio nibbles will be swapped in music_process_channels_tic
			ld a, channel.arpeggio(ix)
			and a, #0xf

		mcf_arp_pos_0: 

		; add the channel's midi note to the arpeggio note
		; double it to make it an offset into the fnum table
		add a, channel.midi_note(ix)
		add a, a

		; add to tone lookup table address in hl
		add a, l
		ld l, a
		adc a, h
		sub a, l
		ld h, a

		; get frequency from table into de
		ld e, (hl)
		inc hl
		ld d, (hl)
		
		ret 

	mcf_no_arp: 
	
	; get tone fnum in de
	ld e, channel.freq(ix)
	ld d, channel.freq+1(ix)

	ret 

; write to pitch registers for the specified channel
; ix: channel
; iy: state
music_update_pitch_registers: 

	push bc
	
	; check which type of channel we're looking at
	ld a, channel.type(ix)
	or a, a
	jr nz, music_update_pitch_is_opll

		; check if this is the noise channel (need to shift the 3 by 5 as subchannel is pre-shifted)
		ld a, channel.subchannel(ix)
		cp a, #3<<5
		jr nz, mup_sn_square_channel

			; we're in a noise channel
			; check whether it's pitched or fixed
			ld a, music_state.noise_mode(iy)
			and a, #0x3
			cp a, #0x3
			jr nz, mup_sn_fixed_noise_channel

				; we're in a pitched noise channel
				; we want to write to the last square channel's pitch instead
				ld c, #0x2<<5
				jr mup_sn_pitched_noise_channel

		; for a regular square channel we update this channel's pitch
		mup_sn_square_channel: 

			; get channel number (pre-shifted into correct position)
			ld c, channel.subchannel(ix)

		; for pitched noise it uses channel 3's pitch
		mup_sn_pitched_noise_channel: 
		
			; get note fnum in de
			ld hl, #sn_tone_lookup
			call music_calc_fnum

			; store tone data in channel struct
			ld channel.freq(ix), e
			ld channel.freq+1(ix), d

			; prepare note value for writing to chip
			; isolate lower nibble
			ld a, e

			and a, #0xf
			; set latch bit
			or a, #0x80
			; combine with channel number
			or a, c
			
			; output first byte of tone data
			ld c, channel.port(ix)
			out (c), a
			
			; get upper four bits of lower byte of fnum
			; or with lower two bits of upper byte of fnum
			ld a, e
			and a, #0xf0
			or a, d

			; rotate them into place
			rlca 
			rlca 
			rlca 
			rlca 

			; output second byte of tone data
			out (c), a
			
			; done
			jr music_update_pitch_done
			
		; pitch not based on channel 3
		mup_sn_fixed_noise_channel: 
			
			; get midi number
			ld a, channel.midi_note(ix)
						
			; mod 12
			fixed_noise_mod_12: 
				sub a, #12
				jr nc, fixed_noise_mod_12
			
			; restrict to 0-2
			add a, #12
			and a, #0x3
			ld c, a
			; actual value should be 2 minus this value
			ld a, #2
			sub a, c
			and a, #0x3
			ld c, a
		
			; clear bottom 2 bits of current noise mode
			; and or with new value
			ld a, music_state.noise_mode(iy)
			or a, c
			
			; update chip
			ld c, channel.port(ix)
			out (c), a
		
			; done
			jr music_update_pitch_done

	music_update_pitch_is_opll: 

		; don't update pitch for drum channels
		cp a, #CHAN_OPLL_DRUMS
		jr z, music_update_pitch_done

			; get note fnum in de
			ld hl, #fm_tone_lookup
			call music_calc_fnum

			; get f-number register to write to for this channel
			ld a, channel.subchannel(ix)
			add a, #0x10
			ld c, a
			out (#OPLL_REG_PORT), a
			
			; lower 8 bits of f number
			ld a, e
			out (#OPLL_DATA_PORT), a
			
			; delay after opll data write
			push hl
			pop hl
			push hl
			pop hl
			push hl
			pop hl

			; store for later
			ld channel.freq(ix), a
			
			; get octave + key on register to write to for this channel
			; by adding to base_reg from earlier
			ld a, c
			add a, #0x10
			out (#OPLL_REG_PORT), a
			
			; get octave and 9th bit of f number
			ld a, d
			
			; store for later
			ld channel.freq+1(ix), a
			
			; set key on bit and write
			and a, #0xf
			or a, #0x10
			out (#OPLL_DATA_PORT), a
			
			nop 
			nop 
			nop 
			nop 
			nop 
			nop 
			nop 
			nop 

			jr music_update_pitch_done


	music_update_pitch_done: 

	; reset pitch_changed flag
	res CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

	pop bc

	ret 
