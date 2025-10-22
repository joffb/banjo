
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
banjo_init_opll:

    ld b, 9

    music_init_opll_loop:

        ; default volume
        ld (ix + channel.volume), 0x0f

        ; temp set fm patch to 15
        ld (ix + channel.patch), 0xf0
        
        ; subchannel number
        ld a, 9
        sub a, b
        ld (ix + channel.subchannel), a

        ; silence chip channel
        add a, 0x30
        out (OPLL_REG_PORT), a
        ld a, 0xff
        out (OPLL_DATA_PORT), a

        ; move to next channel
        ld de, _sizeof_channel
        add ix, de

        djnz music_init_opll_loop

    call banjo_opll_write_delay

	; select rhythm trigger register
	ld a, 0xe
	out (OPLL_REG_PORT), a

    ; clear drum note-ons and disable rhythm mode
    xor a, a
    out (OPLL_DATA_PORT), a

    ; store new value
    ld (opll_drum_note_ons), a

    ret


; ix: channel
; iy: music state
banjo_init_sfx_channel_opll:

    ; default volume
    ld (ix + channel.volume), 0x0f

    ; temp set fm patch to 15
    ld (ix + channel.patch), 0xf0

    ; subchannel number
    ld a, (iy + music_state.sfx_subchannel)
    ld (ix + channel.subchannel), a

    ret