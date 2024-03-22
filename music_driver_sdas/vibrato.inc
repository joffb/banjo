
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
	set CHAN_FLAG_BIT_PITCH_CHANGED, channel.flags(ix)

    ; get channel frequency into de
    ld e, channel.freq(ix)
    ld d, channel.freq+1(ix)

    ; get address in jump table for this channel type in hl
    ; lower byte
    ld a, channel.type(ix)
    ld c, a
    add a, a
    add a, c
    add a, #<muvib_jump_table
    ld l, a

    ; upper byte
    adc a, #>muvib_jump_table
    sub a, l
    ld h, a

    ; jump into jump table
    jp (hl)

    muvib_done: 

        ; update channel frequency
        ld channel.freq(ix), e
        ld channel.freq+1(ix), d

        ret 
