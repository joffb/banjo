
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
_banjo_init_opll_drums: 

    ld b, #6

    ; initialise melodic channels
    music_init_opll_drums_loop_1: 

        ; default volume
        ld channel.volume(ix), #0x0f

        ; temp set fm patch to 15
        ld channel.gp1(ix), #0xf0
        
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
    ld hl, #opll_drum_pitch_trigger_masks

    ; initialise rhythm channels
    music_init_opll_drums_loop_2: 

        ; default volume
        ld channel.volume(ix), #0x0f

        ; temp set fm patch to 15
        ld channel.gp1(ix), #0xf0

        ; subchannel is pitch register
        ld a, (hl)
        ld channel.subchannel(ix), a
        inc hl

        ; drum trigger bit
        ld a, (hl)
        ld channel.gp2(ix), a
        inc hl

        ; volume mask
        ld a, (hl)
        ld channel.gp3(ix), a
        inc hl

        ; move to next channel
        ld de, #_sizeof_channel
        add ix, de

        djnz music_init_opll_drums_loop_2

    ; initialise drum volumes
    ld a, #0
    ld b, #3
    ld hl, #opll_drum_volumes

    music_init_opll_drums_loop_3: 

        ld (hl), a
        inc hl

        djnz music_init_opll_drums_loop_3

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
