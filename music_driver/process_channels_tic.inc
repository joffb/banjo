
; banjo sound driver
; Joe Kennedy 2023

; iy: song_state
; ix: channels
music_process_channels_tic:
	
	; number of channels to process
	ld b, (iy + music_state.channel_count)
	
	music_process_channels_tic_loop:
	
		; check that the note is on and the channel isn't muted
		ld a, (ix + channel.flags)
		and a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
		cp a, CHAN_FLAG_NOTE_ON
		jr nz, mpct_next_channel

		; apply volume macro
		bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
		call nz, music_update_volume_macro

		; apply pitch slides/portamento
		bit CHAN_FLAG_BIT_SLIDE, (ix + channel.flags)
		call nz, music_update_pitch_slide

		; apply vibrato
		bit CHAN_FLAG_BIT_VIBRATO, (ix + channel.flags)
		call nz, music_update_vibrato

		; check if there's an active arpeggio
		bit CHAN_FLAG_BIT_ARPEGGIO, (ix + channel.flags)
		call nz, music_update_arpeggio

		; check if we need to update the pitch of this channel
		bit CHAN_FLAG_BIT_PITCH_CHANGED, (ix + channel.flags)
		call nz, music_update_pitch_registers

		mpct_next_channel:

			; next channel
			ld de, _sizeof_channel
			add ix, de

			djnz music_process_channels_tic_loop

			ret