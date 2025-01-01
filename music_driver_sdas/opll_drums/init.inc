
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
_banjo_init_opll_drums: 

    ld b, #6

    ; initialise melodic channels
    music_init_opll_drums_loop_1: 

        ; default volume
        ld channel.volume(ix), #0x00

        ; temp set fm patch to 15
        ld channel.patch(ix), #0xf0
        
        ; subchannel number
        ld a, #6
        sub a, b
        ld channel.subchannel(ix), a

        ; silence chip channels
        add a, #0x30
        out (#OPLL_REG_PORT), a
        ld a, #0xf
        out (#OPLL_DATA_PORT), a

        ; move to next channel
        ld de, #_sizeof_channel
        add ix, de

        djnz music_init_opll_drums_loop_1

    ld b, #5

    ; initialise rhythm channels
    music_init_opll_drums_loop_2: 

        ; default volume
        ld channel.volume(ix), #0x00

        ; temp set fm patch to 15
        ld channel.patch(ix), #0xf0

        ; subchannel number
        ld a, #5
        sub a, b
        ld channel.subchannel(ix), a

        ; move to next channel
        ld de, #_sizeof_channel
        add ix, de

        djnz music_init_opll_drums_loop_2

	; select rhythm trigger register
	ld a, #0xe
	out (#OPLL_REG_PORT), a

    ; clear drum note-ons and set rhythm mode
    ld a, #0x20

    ; update on chip
    out (#OPLL_DATA_PORT), a

    ; store new value
    ld (opll_drum_note_ons), a

    ; initialise drums to variable pitch mode
    ld a, #0
    ld (opll_drum_fixed_pitch_mode), a

    ; set intial drum values
    call music_set_default_opll_drum_pitches

    ret 
