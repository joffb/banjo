
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_init_opl_2op:

    ld b, 9

    music_init_opl_2op_loop:

        ; default volume
        ld (ix + channel.volume), 0x3f

        ; subchannel number
        ld a, 9
        sub a, b
        ld (ix + channel.subchannel), a

        ; send key-offs
        add a, 0xb0
        out (OPL_REG_PORT), a
        xor a, a
        out (OPL_DATA_PORT), a

        ; move to next channel
        ld de, _sizeof_channel
        add ix, de

        djnz music_init_opl_2op_loop

    call banjo_opl_write_delay

	; select rhythm trigger register
	ld a, 0xbd
	out (OPL_REG_PORT), a

    ; clear drum note-ons and disable rhythm mode
    xor a, a
    out (OPL_DATA_PORT), a

    ; store new value
    ld (opll_drum_note_ons), a

    ret


; ix: channel
; iy: music state
banjo_init_sfx_channel_opl_2op:

    ; default volume
    ld (ix + channel.volume), 0x3f

    ; subchannel number
    ld a, (iy + music_state.sfx_subchannel)
    ld (ix + channel.subchannel), a

    ret