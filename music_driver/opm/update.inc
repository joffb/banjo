
; banjo sound driver
; Joe Kennedy 2025

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_opm:

    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm
    call music_update_channel_opm

    ret


; ix: channel
; iy: music state
banjo_update_channel_opm:

    call music_update_channel_opm
    
    ret


; ix: channel
; iy: music state
music_update_channel_opm:

    ; process new line if necessary
    ; returns events in b and flags in c
    ld hl, music_command_jump_table_opm
    call music_process_new_line

    ; handle note-off
    bit CHAN_FLAG_BIT_NOTE_OFF, c
    jp nz, music_note_off_opm
    music_note_off_opm_done:

    ; store new value of flags
    ld (ix + channel.flags), c

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, c
    jr z, mucopm_done

        ; check if we need to update the pitch of this channel
        ld a, b
        and a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_WAVEDUTY_CHANGED 
        jp nz, music_update_pitch_registers_opm
        music_update_pitch_registers_opm_done:

        ; update volume if there's a volume macro or a volume change
        ; or there's been a master volume change event
        ld a, b
        and a, CHAN_EVENT_VOLUME_CHANGE | CHAN_EVENT_VOLUME_MACRO
        jp nz, music_volume_change_opm
        
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_opm
        music_volume_change_opm_done:

        ; clear temporary events
        ld a, b
        and a, CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        ld (ix + channel.events), a

        mucopm_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix, de

            ret
