
; banjo sound driver
; Joe Kennedy 2023

; bc : events and flags (preserve these!)
; de : instruction pointer (preserve this!)
; ix : channel
; iy : music state
mpnl_note_on_sn:

    .ifdef BANJO_3_57MHZ

		; sub 9 from the note number as the 3.57mhz pitch table starts at the note A
		sub a, 9
		jr nc, mpl_no_sn_nc

			xor a, a

		mpl_no_sn_nc:
		
    .endif

	; standard note-on stuff
	call music_note_on

	jp mpnl_command_done


; bc : events and flags (preserve these!)
; ix : channel
; iy : music state
music_note_off_sn:

	; clear note-off bit
	res CHAN_FLAG_BIT_NOTE_OFF, c

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

	
	jp music_note_off_sn_done
