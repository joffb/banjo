
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; switch envelopes on and update envelope bit in channel volume
mpnl_ay_env_on: 

    ; check if the channel is muted
    bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
    jp nz, mpnl_skip_1_byte_command

        push bc

        ld c, channel.port(ix)

        ; set volume/envelope reg for this channel
        ld a, channel.subchannel(ix)
        add a, #8
        out (c), a

        ; get current volume of channel
        inc c

        .ifdef BANJO_MSX
        inc c
        .endif 

        in a, (c)

        ; write new volume with envelope bit set
        or a, #0x10
        
        .ifdef BANJO_MSX
        dec c
        .endif 

        out (c), a

        ; update stored volume on channel with envelope bit set
        set 4, channel.volume(ix)

        pop bc

        jp mpnl_command_done_one_byte_command


; switch envelopes off and update envelope bit in channel volume
mpnl_ay_env_off: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_1_byte_command

        push bc

        ld c, channel.port(ix)

        ; volume/envelope reg for this channel
        ld a, channel.subchannel(ix)
        add a, #8
        out (c), a

        ; get current volume of channel
        inc c

        .ifdef BANJO_MSX
        inc c
        .endif 

        in a, (c)

        ; write new volume with envelope bit cleared
        and a, #0xf
        
        .ifdef BANJO_MSX
        dec c
        .endif 

        out (c), a

        ; update stored volume on channel with envelope bit cleared
        res 4, channel.volume(ix)

        pop bc

        jp mpnl_command_done_one_byte_command


; change shape of envelope
mpnl_ay_env_shape: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        push bc

        ld c, channel.port(ix)

        ; reg envelope shape register
        ld a, #13
        out (c), a

        ; get new shape and write it
        ld a, (de)
        inc c
        out (c), a
        
        pop bc

        jp mpnl_command_done


; set high bit of envelope period
mpnl_ay_env_period_hi: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        push bc

        ld c, channel.port(ix)

        ; reg envelope period high byte
        ld a, #12
        out (c), a

        ; get new high byte and write it
        ld a, (de)
        inc c
        out (c), a

        pop bc

        jp mpnl_command_done


; set low bit of envelope period
mpnl_ay_env_period_lo: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        push bc

        ld c, channel.port(ix)

        ; reg envelope period low byte
        ld a, #11
        out (c), a

        ; get new low byte and write it
        ld a, (de)
        inc c
        out (c), a

        pop bc

        jp mpnl_command_done


; set both high and low bytes of envelope period
mpnl_ay_env_period_word: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_3_byte_command

        push bc

        ld c, channel.port(ix)

        ; reg envelope period low byte
        ld a, #11
        out (c), a

        ; get new low byte and write it
        ld a, (de)
        inc c
        out (#AY_DATA_WRITE), a
        dec c

        ; reg envelope period high byte
        ld a, #12
        out (c), a

        ; get new high byte and write it
        inc de
        ld a, (de)
        inc c
        out (c), a

        pop bc

        jp mpnl_command_done



