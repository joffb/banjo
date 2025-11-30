
; banjo sound driver
; Joe Kennedy 2024

; commands will be jumped to from music_process_new_line

music_ay_channel_mix_masks:
    .db 0xf6
    .db 0xed
    .db 0xdb


mpnl_ay_volume_change:

    ld a, (de)

    ; preserve envelope bit if it's present
    bit 4, (ix + channel.volume)
    jr z, mpnl_ay_vc_env_off

        or a, 0x10

    mpnl_ay_vc_env_off:
    ld (ix + channel.volume), a

    ; set bit to indicate a channel volume change event
    set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

    jp mpnl_command_done


mpnl_ay_channel_mix:

    ; check if the channel is muted
    bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    jp nz, mpnl_skip_2_byte_command

        push bc

        ld c, (ix + channel.port)

        ld a, (ix + channel.subchannel)
        ld hl, music_ay_channel_mix_masks

        ; get address of mix mask into de
        add a, l
        ld l, a
        adc a, h
        sub a, l
        ld h, a

        ; get current mix
        ld a, 7
        out (c), a
        inc c

        .ifdef BANJO_MSX
        inc c
        .endif

        in a, (c)

        ; mask out values
        and a, (hl)

        ; combine with new values
        ex de, hl
        or a, (hl)
        ex de, hl

        ; output
        
        .ifdef BANJO_MSX
        dec c
        .endif

        out (c), a

        pop bc

        jp mpnl_command_done


mpnl_ay_noise_pitch:

    ; check if the channel is muted
    bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    jp nz, mpnl_skip_2_byte_command

        push bc

        ld c, (ix + channel.port)

        ; select noise reg
        ld a, 6
        out (c), a

        ; get noise amount and write it
        ld a, (de)
        inc c
        out (c), a

        pop bc

        jp mpnl_command_done
