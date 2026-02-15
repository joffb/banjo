
; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opl_2op_volume_change:

	; get new volume
    inc de
	ld a, (de)
	and a, 0x3f

	; don't do anything if the volume is the same
	cp a, (ix + channel.volume)
	jp z, mpnl_command_done

		; store new channel volume
		ld (ix + channel.volume), a

		; set bit to indicate a channel volume change event
		set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

		jp mpnl_command_done