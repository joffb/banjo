
; banjo sound driver
; Joe Kennedy 2023

; hl: pointer to music data
; stack: loop mode
banjo_play_song:

	; music pointer into de
	ex de, hl
	; return address into hl
	pop hl
	dec sp
	; swap return address with loop mode
	ex (sp), hl
	; music pointer in hl, loop mode in d
	ex de, hl

	; try to ensure we're playing valid data
	ld a, (hl)
	cp a, BANJO_MAGIC_BYTE
	ret nz

	; get channnel count
	; abort if it's > CHANNEL_COUNT
	inc hl
	inc hl
	ld a, CHANNEL_COUNT
	cp a, (hl)
	ret c

	; move pointer back
	dec hl
	dec hl

	; mark song as playing
	ld a, 1
	ld (song_playing), a

	; loop mode into b
	ld b, d

	push iy
	push ix

	ld ix, song_channels
	ld iy, song_state
	call music_play

	pop ix
	pop iy

	ret

; hl: pointer to music data
; stack: loop mode
banjo_play_sfx:

	; music pointer into de
	ex de, hl
	; return address into hl
	pop hl
	dec sp
	; swap return address with loop mode
	ex (sp), hl
	; music pointer in hl, loop mode in d
	ex de, hl

	; try to ensure we're playing valid data
	ld a, (hl)
	cp a, BANJO_MAGIC_BYTE
	ret nz

	; mark sfx as playing
	ld a, 1
	ld (sfx_playing), a

	; loop mode into b
	ld b, d

	push iy
	push ix

	ld ix, sfx_channel
	ld iy, sfx_state
	call music_play

	pop ix
	pop iy

	ret

; b: loop mode
; c: bank
; ix: channels
; iy: state
; hl: pointer to music data
music_play:
	
	push bc

	.ifndef QUEUES_OFF
	; change bank
	ld a, c
	ld (SLOT_2_BANK_CHANGE), a
	.endif

	; copy over song header
	; at the end of the song header is the 32-byte channel types array
	; don't copy that just yet
	ld b, _sizeof_music_state - 32 
	
	; put music state header address into de
	push iy
	pop de

	music_play_copy_header:
		
		ld a, (hl)
		ld (de), a
		
		inc hl
		inc de
		
		djnz music_play_copy_header
	
	; copy over channel types array
	; but only for the active channels
	ld a, (iy + music_state.channel_count)
	add a, a
	ld b, a

	music_play_copy_channel_types:

		ld a, (hl)
		ld (de), a
		
		inc hl
		inc de

		djnz music_play_copy_channel_types

	pop bc

	; store song bank in music state
	ld (iy + music_state.bank), c

	; check if the loop mode has been overwritten
	ld a, b
	cp a, 0xff
	jr z, mp_loop_mode_unchanged

		ld (iy + music_state.loop), a

	mp_loop_mode_unchanged:

	; init song position variables
	; check if we need to adjust the playback speeds from ntsc -> pal
	ld a, (music_framerate)
	cp a, 60
	jr z, music_play_same_speed
		
		; subtract 1 from speed 1 and speed 2 to roughly match playback
		; playing back on pal rather than ntsc
		ld a, (iy + music_state.speed_1)
		dec a
		ld (iy + music_state.speed_1), a
		
		ld a, (iy + music_state.speed_2)
		dec a
		ld (iy + music_state.speed_2), a

		
	music_play_same_speed:
	
	; preserve channel data pointer
	push ix 

	; point hl at the order_ptrs in the music_state
	push iy
	pop hl
	ld bc, music_state.order_ptrs
	add hl, bc

	; point hl at the first channel's order pointer
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; init channel data
	ld a, (iy + music_state.channel_count)
	ld b, a
		
	music_play_channel_data:

		ld a, 0

		; default to not muted
		; default legato off and note off
		; default pitch to not require updating
		ld (ix + channel.flags), a

		; default volume
		ld (ix + channel.volume), a

		; default slide variables to 0
		ld (ix + channel.slide_amount), a
		ld (ix + channel.slide_type), a
		
		; default volume macro length to 0
		ld (ix + channel.volume_macro_len), a
		
		; default line wait to 0
		ld (ix + channel.line_wait), a

		; default arpeggios to 0
		ld (ix + channel.arpeggio), a

		; default vibrato_current and vibrato_target to VIBRATO_CENTRE (equivalent to 0 vibrato)
		ld (ix + channel.vibrato_target), VIBRATO_CENTRE
		ld (ix + channel.vibrato_current), VIBRATO_CENTRE

		; the following variables are initialised to 0xff
		ld a, 0xff

 		; default arpeggios off with arpeggio_pos = 0xff
		ld (ix + channel.arpeggio_pos), a

		; default instrument to no instrument (0xff)
		ld (ix + channel.instrument_num), a

		; initialise freq to 0xffff
		ld (ix + channel.freq), a
		ld (ix + channel.freq + 1), a

		; init pointer to order table for this channel
		; get pointer to order data in de
		ld e, (hl)
		ld (ix + channel.order_table_ptr), e
		inc hl
		ld d, (hl)
		ld (ix + channel.order_table_ptr + 1), d
		inc hl
		
		; init address of first pattern in channel
		ld a, (de)
		ld (ix + channel.pattern_ptr), a
		inc de
		ld a, (de)
		ld (ix + channel.pattern_ptr + 1), a
		
		; move to next channel
		ld de, _sizeof_channel
		add ix, de
		djnz music_play_channel_data
	
	; restore channel data pointer
	pop ix

	;
	; init channel type data
	;

	; preserve channel data pointer
	push ix

	; point hl at channel_types
	push iy
	pop hl
	ld bc, music_state.channel_types
	add hl, bc
	
	; go through each channel and set its channel type
	ld b, (iy + music_state.channel_count)
	
	music_play_channel_types:
		
		; temp set fm patch to 15
		ld a, 0xf0
		ld (ix + channel.fm_patch_shifted), a

		; type of channel (sn/fm/fm drums)
		ld a, (hl)
		ld (ix + channel.type), a
		
		inc hl
		
		; subchannel within chip
		; e.g. if there's sn and fm, this would be 0 for the first channels 
		; of both the sn and fm chips
		; if this is an SN channel, we pre-shift it to save work later
		cp a, CHAN_SN76489
		ld a, (hl)
		jr nz, mpct_dont_shift_subchannel

			; keep subchannel around in c
			ld c, a

			; if the upper nibble isn't empty, we're writing to the second SN chip
			and a, 0xf0
			jr z, mpct_sn_chip_1

				ld a, SN76489_2_PORT
				jr mpct_sn_chip_2

			mpct_sn_chip_1:
			ld a, SN76489_PORT

			mpct_sn_chip_2:
			; store the port number 
			ld (ix + channel.port), a

			; restore subchannel, clean up the upper nibble and preshift it
			ld a, c
			and a, 0xf
			rrca
			rrca
			rrca

			; preserve shifted subchannel in c
			ld c, a

			; set default channel volume
			or a, 0x90
			ld (ix + channel.volume), a

			; restore shifted subchannel
			ld a, c

		mpct_dont_shift_subchannel:
		ld (ix + channel.subchannel), a

		inc hl
		
		; set fnum/octave/volume registers and drum trigger byte for drum tracks
		ld a, (ix + channel.type)
		cp a, CHAN_OPLL_DRUMS
		jr nz, music_play_channel_types_next_channel
		
			; get subchannel # and store for later
			ld a, (ix + channel.subchannel)
			ld c, a
			
			; look up whether volume should be shifted
			ld de, fm_drum_volume_masks
			ld a, c
			add a, e
			ld e, a
			adc a, d
			sub a, e
			ld d, a
			ld a, (de)
			
			; store
			ld (ix + channel.fm_drum_volume_mask), a
			
			; look up base for fnum/octave/volume registers
			ld de, fm_drum_pitch_registers
			ld a, c
			add a, e
			ld e, a
			adc a, d
			sub a, e
			ld d, a
			ld a, (de)
			
			; store it in channel.port
			ld (ix + channel.port), a
			
			; look up trigger byte
			ld de, fm_drum_triggers
			ld a, c
			add a, e
			ld e, a
			adc a, d
			sub a, e
			ld d, a
			ld a, (de)
			
			; store trigger
			ld (ix + channel.fm_drum_trigger), a

		music_play_channel_types_next_channel:

		ld de, _sizeof_channel
		add ix, de
		
		dec b
		jp nz, music_play_channel_types
		
	; restore channel data pointer
	pop ix

	; for songs we should clear down all the channels and reset things
	ld a, (iy + music_state.sfx_channel)
	cp a, 0xff
	jr nz, music_play_is_sfx

		; default sn noise to use ch3 tone and white noise
		ld a, 0xe7
		out (SN76489_PORT), a
		out (SN76489_2_PORT), a

		; check if we're in game gear mode
		ld a, (banjo_game_gear_mode)
		cp a, 1
		jr nz, mp_not_game_gear

			; default all channels to center panning
			ld a, 0xff
			ld (iy + music_state.panning), a
			out (GAME_GEAR_PAN_PORT), a

		mp_not_game_gear:

		; mute all sn channels
		call music_mute_all_sn

		; init fm_drum_note_ons as no drums playing
		ld a, 0
		ld (fm_drum_note_ons), a

		; max rhythm values	
		ld c, 0x36
		ld b, 3
		
		music_play_fm_rhythm_vols:
			ld a, c
			out (OPLL_REG_PORT), a
			ld a, 0xff
			out (OPLL_DATA_PORT), a
			inc c

			; delay after opll data write
			push hl
			pop hl
			push hl
			pop hl
			push hl
			pop hl

			djnz music_play_fm_rhythm_vols

		; set up default rhythm instrument values
		ld a, (iy + music_state.has_fm_drums)
		or a, a
		call z, music_set_default_fm_drum_pitches

		; mute all fm channels
		call music_mute_all_fm

		; unmute SN and FM
		ld a, 0x3
		out (AUDIO_CONTROL_PORT), a

		jr music_play_done
	
	; for sfx we just need to mute a channel
	music_play_is_sfx:

		ld a, 0x1
		ld (sfx_playing), a

		ld a, (iy + music_state.sfx_channel)
		call banjo_mute_song_channel

	music_play_done:
	
		; play first line of track
		call music_process_new_line

		ret

banjo_song_rewind:

	; check the song state to see if the first byte is the magic byte
	ld a, (song_state + music_state.magic_byte)
	cp a, BANJO_MAGIC_BYTE
	jr nz, bsrw_done

		ld ix, song_channels
		ld iy, song_state
		call music_reset_channels_order_and_pattern

		ld a, 0
		ld (iy + music_state.tic), a
		ld (iy + music_state.subtic), a
		ld (iy + music_state.line), a
		ld (iy + music_state.order), a

		ld b, (iy + music_state.channel_count)
		bsrw_channel_loop:

			ld a, 0
			ld (ix + channel.flags), 0
			ld (ix + channel.line_wait), 0

			ld de, _sizeof_channel
			add ix, de
			djnz bsrw_channel_loop

	bsrw_done:
	ret

; if there's a valid banjo song loaded, resume playback of it
banjo_song_resume:

	; check the song state to see if the first byte is the magic byte
	ld a, (song_state + music_state.magic_byte)
	cp a, BANJO_MAGIC_BYTE
	jr nz, bsr_no_valid_song

		; resume playback
		ld a, 1
		ld (song_playing), a
	
	bsr_no_valid_song:
	ret
