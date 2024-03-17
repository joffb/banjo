
; banjo sound driver
; Joe Kennedy 2023

; iy: song_state
; ix: channels
music_process_channels_tic: 
	
	; number of channels to process
	ld b, music_state.channel_count(iy)
	
	music_process_channels_tic_loop: 
	
		; check if this channel is currently playing
		bit CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)
		jp z, mpct_next_channel

		; check if this channel is currently playing
		bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
		jp nz, mpct_next_channel

		; apply volume macro
		bit CHAN_FLAG_BIT_VOLUME_MACRO, channel.flags(ix)
		call nz, music_update_volume_macro

		; apply pitch slides/portamento
		bit CHAN_FLAG_BIT_SLIDE, channel.flags(ix)
		call nz, music_update_pitch_slide

		; apply vibrato
		bit CHAN_FLAG_BIT_VIBRATO, channel.flags(ix)
		call nz, music_update_vibrato

		; check if there's an active arpeggio
		bit CHAN_FLAG_BIT_ARPEGGIO, channel.flags(ix)
		call nz, music_update_arpeggio

		; check if we need to update the pitch of this channel
		bit CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)
		call nz, music_update_pitch_registers

		mpct_next_channel: 

			; next channel
			ld de, #_sizeof_channel
			add ix, de
			
			dec b
			jp nz, music_process_channels_tic_loop

			ret 
