
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

; switch envelopes on and update envelope bit in channel volume
mpnl_ay_env_on: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_1_byte_command

        ; set volume/envelope reg for this channel
        ld a, channel.subchannel(ix)
        add a, #8
        out (#AY_REG_WRITE), a

        ; get current volume of channel
        in a, (#AY_DATA_READ)

        ; write new volume with envelope bit set
        or a, #0x10
        out (#AY_DATA_WRITE), a

        ; update stored volume on channel with envelope bit set
        set 4, channel.volume(ix)

        jp mpnl_command_done_one_byte_command


; switch envelopes off and update envelope bit in channel volume
mpnl_ay_env_off: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_1_byte_command

        ; volume/envelope reg for this channel
        ld a, channel.subchannel(ix)
        add a, #8
        out (#AY_REG_WRITE), a

        ; get current volume of channel
        in a, (#AY_DATA_READ)

        ; write new volume with envelope bit cleared
        and a, #0xf
        out (#AY_DATA_WRITE), a

        ; update stored volume on channel with envelope bit cleared
        res 4, channel.volume(ix)

        jp mpnl_command_done_one_byte_command


; change shape of envelope
mpnl_ay_env_shape: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        ; reg envelope shape register
        ld a, #13
        out (#AY_REG_WRITE), a

        ; get new shape and write it
        ld a, (de)
        out (#AY_DATA_WRITE), a
        
        jp mpnl_command_done


; set high bit of envelope period
mpnl_ay_env_period_hi: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        ; reg envelope period high byte
        ld a, #12
        out (#AY_REG_WRITE), a

        ; get new high byte and write it
        ld a, (de)
        out (#AY_DATA_WRITE), a

        jp mpnl_command_done


; set low bit of envelope period
mpnl_ay_env_period_lo: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_2_byte_command

        ; reg envelope period low byte
        ld a, #11
        out (#AY_REG_WRITE), a

        ; get new low byte and write it
        ld a, (de)
        out (#AY_DATA_WRITE), a

        jp mpnl_command_done


; set both high and low bytes of envelope period
mpnl_ay_env_period_word: 

	; check if the channel is muted
	bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
	jp nz, mpnl_skip_3_byte_command

        ; reg envelope period low byte
        ld a, #11
        out (#AY_REG_WRITE), a

        ; get new low byte and write it
        ld a, (de)
        out (#AY_DATA_WRITE), a

        ; reg envelope period high byte
        ld a, #12
        out (#AY_REG_WRITE), a

        ; get new high byte and write it
        inc de
        ld a, (de)
        out (#AY_DATA_WRITE), a

        jp mpnl_command_done



