
; banjo sound driver
; Joe Kennedy 2024

; handle note-on stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_on:

	; mark pitch and volume as requiring update
	ld (ix + channel.events), CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE

	; set note-on bit
    set CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	; if legato is on, we don't need to reset the macro
	bit CHAN_FLAG_BIT_LEGATO, (ix + channel.flags)
	ret nz

		; reset macro positions
		ld (ix + channel.volume_macro_pos), 0
		ld (ix + channel.ex_macro_pos), 0

		ret

; handle note-off stuff common between all channel types
; b: channel number
; ix: channel
; iy: music_state
music_note_off:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

	ret