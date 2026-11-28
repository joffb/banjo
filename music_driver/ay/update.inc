
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

    ld hl, music_command_jump_table_ay
    call music_process_new_line

    ; handle note-off
    bit CHAN_FLAG_BIT_NOTE_OFF, c
    jp nz, music_note_off_ay
    music_note_off_ay_done:

    ; store new value of flags
    ld (ix + channel.flags), c

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, c
    jr z, mucay_done
        
        ; change of noise duty?
        bit CHAN_FLAG_BIT_EX_MACRO, c
        jr z, muc_ay_no_duty

            ; after this, hl will be pointing at the new ex macro value
            call music_update_ex_macro

            ld c, (ix + channel.port)

            ; select noise reg
            ld a, 6
            out (c), a

            ; get noise amount and write it
            ld a, (hl)
            inc c
            out (c), a

        muc_ay_no_duty:

        ; check if we need to update the pitch of this channel
        ld a, b
        and a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE | CHAN_EVENT_ARP_MACRO
        jp nz, music_update_pitch_registers_ay
        music_update_pitch_registers_ay_done:

        ; update volume if there's been a master volume change event
        ; or there's a volume macro or a volume change
        ld a, b
        and a, CHAN_EVENT_VOLUME_CHANGE | CHAN_EVENT_VOLUME_MACRO
        jp nz, music_volume_change_ay
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_ay      
        music_volume_change_ay_done:

        ; clear temporary events
        ld a, b
        and a, CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        ld (ix + channel.events), a
        
        mucay_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix, de
            
            ret

