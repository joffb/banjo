
; banjo sound driver
; Joe Kennedy 2023

; b  : current channel number
; de : instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel
; iy : music state
mpnl_note_on_sn:

	; standard note-on stuff
	ld hl, sn_tone_lookup
	call music_note_on

	jp mpnl_command_done

; de: instruction pointer - needs to be exhanged with hl by the time we return
; ix : channel
; iy : music state
mpnl_note_off_sn:

	; standard note-off stuff
	call music_note_off

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_sn_dont_write_chip

		; get channel (already shifted to the correct position)
		ld a, (ix + channel.subchannel)

		; preserve c
		ld l, c

		; set latch = 1, type = volume, volume = 0
		or a, 0x9f
		ld c, (ix + channel.port)
		out (c), a

		; restore c
		ld l, c

        ; ALF TEST
        ; additional write delay
		.ifdef BANJO_ALF
			call banjo_sn_alf_wait
		.endif

	mnoff_sn_dont_write_chip:
	
	jp mpnl_command_done_one_byte_command
