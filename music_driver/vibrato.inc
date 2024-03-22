
; banjo sound driver
; Joe Kennedy 2024

muvib_jump_table:

    jp music_update_vibrato_sn
    jp music_update_vibrato_fm
    jp muvib_done

; ix: current channel
; iy: music state
music_update_vibrato:

	; flag that we need to update the pitch
	set CHAN_FLAG_BIT_PITCH_CHANGED, (ix + channel.flags)

    ; get channel frequency into de
    ld e, (ix + channel.freq)
    ld d, (ix + channel.freq + 1)

    ; get address in jump table for this channel type in hl
    ; lower byte
    ld a, (ix + channel.type)
    ld c, a
    add a, a
    add a, c
    add a, <muvib_jump_table
    ld l, a

    ; upper byte
    adc a, >muvib_jump_table
    sub a, l
    ld h, a

    ; jump into jump table
    jp (hl)

    muvib_done:

        ; update channel frequency
        ld (ix + channel.freq), e
        ld (ix + channel.freq + 1), d

        ret    	
