
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opn_volume_change:

	; get new volume
    inc de
	ld a, (de)
	and a, 0x7f

	; don't do anything if the volume is the same
	cp a, (ix + channel.volume)
	jp z, mpnl_command_done

		; store new channel volume
		ld (ix + channel.volume), a

		; set bit to indicate a channel volume change event
		set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

		jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opn_pan:

	; check if the channel is muted and skip command if it is
	bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
	jp nz, mpnl_skip_2_byte_command

		; get pan value
		ld a, (de)
		ld l, a

		; get alg/fb register value
		ld a, (ix + channel.gp1)
		and a, 0x3f
		or a, l
		ld l, a

		ld h, 0xb4
		call banjo_opn_write_auto

		jp mpnl_command_done

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opn_lfo:

	; get lfo value
	ld a, (de)
	and a, 0x1f

    ld h, 0x22
    ld l, a
    call banjo_opn_write

	jp mpnl_command_done