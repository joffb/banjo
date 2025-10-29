
; banjo sound driver
; Joe Kennedy 2023

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : channel
; iy : music state
mpnl_note_on_sn:

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
; ix : channel
; iy : music state
mpnl_note_off_sn:

	; clear note-on bit
    res CHAN_FLAG_BIT_NOTE_ON, (ix + channel.flags)

    ; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jr nz, mnoff_sn_dont_write_chip

		; get channel (already shifted to the correct position)
		ld a, (ix + channel.subchannel)

		; set latch = 1, type = volume, volume = 0
		or a, 0x9f

		; write to chip
		.ifdef BANJO_SMS

			out (SN76489_PORT), a

		.else

			; preserve c
			ld l, c

			ld c, (ix + channel.port)
			out (c), a

			; restore c
			ld l, c
			
		.endif


        ; ALF TEST
        ; additional write delay
		.ifdef BANJO_ALF
			call banjo_sn_alf_wait
		.endif

	mnoff_sn_dont_write_chip:
	
	jp mpnl_command_done_one_byte_command
