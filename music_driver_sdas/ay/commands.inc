
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

music_ay_channel_mix_masks: 
    .db 0xf6
    .db 0xed
    .db 0xdb


mpnl_ay_volume_change: 

    ; restore original hl
    ex de, hl

    ; store new channel volume
    inc hl
    ld a, (hl)

    ; preserve envelope bit if it's present
    bit 4, channel.volume(ix)
    jr z, mpnl_ay_vc_env_off

        or a, #0x10

    mpnl_ay_vc_env_off: 
    ld channel.volume(ix), a

    ; set bit to indicate a channel volume change event
    set CHAN_EVENT_BIT_VOLUME_CHANGE, channel.events(ix)

    inc hl
    jp mpnl_command_done

    mpnl_ay_channel_mix: 

    ; check if the channel is muted
    bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
    jp nz, mpnl_skip_2_byte_command

    ; restore original hl
    ex de, hl

    ld a, channel.subchannel(ix)
    ld de, #music_ay_channel_mix_masks

    ; get address of mix mask into de
    add a, e
    ld e, a
    adc a, d
    sub a, e
    ld d, a

    ; get mix mask and store in c
    ld a, (de)
    ld c, a

    ; get current mix
    ld a, #7
    out (#AY_REG_WRITE), a
    in a, (#AY_DATA_READ)

    ; mask out values
    and a, c

    ; combine with new values
    inc hl
    or a, (hl)

    ; output
    out (#AY_DATA_WRITE), a

    inc hl
    jp mpnl_command_done


mpnl_ay_noise_pitch: 

    ; check if the channel is muted
    bit CHAN_FLAG_BIT_MUTED, channel.flags(ix)
    jp nz, mpnl_skip_2_byte_command

    ; restore original hl
    ex de, hl

    ; select noise reg
    ld a, #6
    out (#AY_REG_WRITE), a

    ; get noise amount and write it
    inc hl
    ld a, (hl)
    out (#AY_DATA_WRITE), a

    inc hl
    jp mpnl_command_done
