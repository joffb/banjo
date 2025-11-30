
; banjo sound driver
; Joe Kennedy 2024

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_ay:

    call music_update_channel_ay
    call music_update_channel_ay
    call music_update_channel_ay

    ret


; ix: channel
; iy: music state
banjo_update_channel_ay:

    call music_update_channel_ay

    ret


; ix: channel
; iy: music state
music_update_channel_ay:

    ld bc, music_command_jump_table_ay
    call music_process_new_line

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, b
    jr z, mucay_done

        ; process volume macro
        bit CHAN_FLAG_BIT_VOLUME_MACRO, b
        call nz, music_update_volume_macro

        ; process arp macro
        bit CHAN_FLAG_BIT_ARP_MACRO, b
        call nz, music_update_arp_macro
        
        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, b
        call nz, music_update_ex_macro
        
        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, (ix + channel.events)

        ; change of noise duty?
        bit CHAN_EVENT_BIT_DUTY_CHANGED, b
        jr z, muc_ay_no_duty

            ld c, (ix + channel.port)

            ; select noise reg
            ld a, 6
            out (c), a

            ; get noise amount and write it
            ld a, (ix + channel.ex_macro_val)
            inc c
            out (c), a

        muc_ay_no_duty:

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_PITCH_CHANGE_ALWAYS
        and a, b
        jp nz, music_update_pitch_registers_ay
        music_update_pitch_registers_ay_done:

        ; update volume if there's been a volume change event
        ; or the master volume changed
        ld a, b
        or a, (iy + music_state.flags)
        and a, CHAN_EVENT_VOLUME_CHANGE
        jp nz, music_volume_change_ay      
        music_volume_change_ay_done:

        ; clear events
		ld a, b
		and a, CHAN_EVENT_PITCH_CHANGE_ALWAYS | CHAN_EVENT_MACRO_RELEASE | CHAN_EVENT_LEGATO | CHAN_EVENT_TIC_WAIT
        ld (ix + channel.events), a
        
        mucay_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix, de
            
            ret

