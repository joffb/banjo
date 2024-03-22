
; banjo sound driver
; Joe Kennedy 2023

music_update_pitch_slide_jump_table: 

	; sn slide type jump table
	jp music_update_pitch_slide_done
	jp music_update_sn_pitch_slide_upward
	jp music_update_sn_pitch_slide_downward
	jp music_update_sn_portamento

	; fm slide type jump table
	jp music_update_pitch_slide_done
	jp music_update_fm_pitch_slide_upward
	jp music_update_fm_pitch_slide_downward
	jp music_update_fm_portamento

	; fm drums slide type jump table
	jp music_update_pitch_slide_done
	jp music_update_pitch_slide_done
	jp music_update_pitch_slide_done
	jp music_update_pitch_slide_done

; update the pitch slides or portamento for a channel
; ix: channel
; iy: state
music_update_pitch_slide: 

	; flag that we need to update the pitch
	set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

	; get current freq into de
	ld e, channel.freq(ix)
	ld d, channel.freq+1(ix)

	; offset into jump table by channel.type * 4
	ld a, channel.type(ix)
	rlca 
	rlca 

	; offset into jump table by slide type
	add a, channel.slide_type(ix)

	; triple it as jump instructions are 3 bytes in size
	ld c, a
	add a, a
	add a, c

	; get address in table to jump to
	add a, #<music_update_pitch_slide_jump_table
	ld l, a
	adc a, #>music_update_pitch_slide_jump_table
	sub a, l
	ld h, a

	; jump to it
	jp (hl)

	music_update_pitch_slide_done: 

		; update freq with new value
		ld channel.freq(ix), e
		ld channel.freq+1(ix), d

		ret 
