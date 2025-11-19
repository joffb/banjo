
; banjo sound driver
; Joe Kennedy 2025

; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opm_volume_change:

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
mpnl_opm_lfo_rate:

	; get lfo value
	ld a, (de)
	ld l, a
	ld h, 0x18
	call banjo_opm_write

	jp mpnl_command_done


; bc : jump table address (preserve this!)
; de : instruction pointer (preserve this!)
mpnl_opm_lfo_wave:

	; get wave value
	ld a, (de)
	ld l, a
	ld h, 0x1b
	call banjo_opm_write

	jp mpnl_command_done