
; banjo sound driver
; Joe Kennedy 2023

; iy: song_state
; ix: channels
music_process_channels_tic: 
	
	; number of channels to process
	ld b, music_state.channel_count(iy)
	
	music_process_channels_tic_loop: 
	
		; check if this channel is currently playing
		ld a, channel.flags(ix)
		and a, #CHAN_FLAG_NOTE_ON
		jp z, mpct_next_channel

		; check if this channel is currently playing
		ld a, channel.flags(ix)
		and a, #CHAN_FLAG_MUTED
		jp nz, mpct_next_channel

		; check if there's a volume macro to process
		ld a, channel.volume_macro_len(ix)
		or a, a
		jr z, mpct_no_volume_macro
		
			; keep volume macro length in c
			ld c, a

			; get current pointer to volume macro into hl
			ld l, channel.volume_macro_ptr(ix)
			ld h, channel.volume_macro_ptr+1(ix)
			
			; check if we've reached the end of the macro and do nothing if we have
			ld a, channel.volume_macro_pos(ix)
			cp a, c
			jr nz, mpct_volume_macro_continue

				; if loop == 0xff there's no loop so we're done
				ld a, channel.volume_macro_loop(ix)
				cp a, #0xff
				jr z, mpct_no_volume_macro

				; keep loop value to subtract in c
				ld c, a

				; store new volume macro position
				ld a, channel.volume_macro_pos(ix)
				sub a, c
				ld channel.volume_macro_pos(ix), a

				; get lower byte of volume macro pointer in a
				ld a, l

				; subtract loop point from current pointer
				; to get new volume_macro_ptr
				sub a, c
				ld l, a
				ld a, h
				sbc a, #0
				ld h, a

			mpct_volume_macro_continue: 
			
			; load new volume level into d
			ld d, (hl)
			
			; check channel type
			ld a, channel.type(ix)
			
			; TODO: apply volume macro to drums
			cp a, #CHAN_OPLL_DRUMS
			jr z, mpct_volume_macro_done
			
			; set SN channel volume 
			cp a, #CHAN_SN76489
			jr nz, mpct_opll_channel
			
				; get channel (pre-shifted into correct position)
				ld a, channel.subchannel(ix)

				; set latch and volume bit and put volume in lower 4 bits
				or a, #0x90
				or a, d
				
				; load the port into c
				ld c, channel.port(ix)
				; update chip
				out (c), a
			
				jr mpct_volume_macro_done
			
			; set OPLL channel volume
			mpct_opll_channel: 
			cp a, #CHAN_OPLL
			jr nz, mpct_volume_macro_done
			
				; choose volume register
				ld a, channel.fm_base_reg(ix)
                add a, #0x20
				ld (OPLL_REG_PORT), a
				
				; get current patch which is the upper nibble of the volume reg
				; and bombine with new volume level
				ld a, channel.fm_patch_shifted(ix)
				or a, d
				
				; write new value
				ld (OPLL_DATA_PORT), a
				
		mpct_volume_macro_done: 
		
			; move volume macro position along and save it
			inc channel.volume_macro_pos(ix)
			
			; move volume macro pointer along and save it
			inc hl
			ld channel.volume_macro_ptr(ix), l
			ld channel.volume_macro_ptr+1(ix), h
			
		mpct_no_volume_macro: 

		; slide type == 0, no slide occuring
		ld a, channel.slide_type(ix)
		or a, a
		jp z, mpct_check_arpeggio

		; only apply pitch slides or portamento if slide_amount > 0
		ld a, channel.slide_amount(ix)
		or a, a
		jp nz, music_update_pitch_slide

		; check if there's an active arpeggio
		mpct_check_arpeggio: 
		ld a, channel.arpeggio_pos(ix)
		cp a, #0xff
		jr z, music_process_no_arpeggio

			; move arpeggio pos along
			inc a
			cp a, #3
			jr nz, mpct_process_arpeggio_pos

				; reset arpeggio position to 0
				ld a, #0

			mpct_process_arpeggio_pos: 
			ld channel.arpeggio_pos(ix), a

			; flag that we need to update the pitch
			ld a, channel.flags(ix)
			or a, #CHAN_FLAG_PITCH_CHANGED
			ld channel.flags(ix), a

		music_process_no_arpeggio: 

		; check if we need to update the pitch of this channel
		ld a, channel.flags(ix)
		and a, #CHAN_FLAG_PITCH_CHANGED
		call nz, music_update_pitch_registers

		mpct_next_channel: 

		; next channel
		ld de, #_sizeof_channel
		add ix, de
		
		dec b
		jp nz, music_process_channels_tic_loop

	ret 
