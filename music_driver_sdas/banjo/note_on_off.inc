
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_on: 

	; mark pitch and volume as requiring update
	ld channel.events(ix), #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_VOLUME_CHANGE

	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	; if legato is on, we don't need to reset the macro
	bit CHAN_FLAG_BIT_LEGATO, channel.flags(ix)
	ret nz

		; reset macro positions
		xor a
		ld channel.volume_macro_pos(ix), a
		ld channel.ex_macro_pos(ix), a
		ld channel.vibrato_counter(ix), a

		ret 

; handle note-off stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_off: 

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, channel.flags(ix)

	ret 
