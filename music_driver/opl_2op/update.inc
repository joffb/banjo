
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_opl_2op:

    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op
    call music_update_channel_opl_2op

    ret


; ix: channel
; iy: music state
banjo_update_channel_opl_2op:

    call music_update_channel_opl_2op
    
    ret


; delay after writing to opll chip
banjo_opl_write_delay:
    push hl
    pop hl
    push hl
    pop hl
    ret


; a: subchannel number
; turn the subchannel number into an operator slot index
; channel | mod slot index | carrier slot index |
;       0 |              0 |                  3 |
;       1 |              1 |                  4 |
;       2 |              2 |                  5 |
;       3 |              8 |                 11 |
;       4 |              9 |                 12 |
;       5 |             10 |                 13 |
;       6 |             16 |                 19 |
;       7 |             17 |                 20 |
;       8 |             18 |                 21 |
banjo_opl_2op_get_first_slot_index:

    ; channel number >= 6
    cp a, 6
    jr c, bogfsi_ch_under_6

        ; add 16 to channel number
        ; operator slots start at 16
        add a, 10
        ret

    ; channel number >= 3
    bogfsi_ch_under_6:
    cp a, 3
    ret c

        ; add 8 to channel number
        ; operator slots start at 8
        add a, 5
        ret


.ifndef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_opl_2op:

    ; process new line if necessary
    ; returns events in b and flags in c
    ld hl, music_command_jump_table_opl_2op
    call music_process_new_line

    ; handle note-off
    bit CHAN_FLAG_BIT_NOTE_OFF, c
    jp nz, music_note_off_opl_2op
    music_note_off_opl_2op_done:

    ; store new value of flags
    ld (ix + channel.flags), c

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, c
    jr z, muco2_done

        ; check if we need to update the pitch of this channel
        ld a, b
        and a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE | CHAN_EVENT_ARP_MACRO
        jp nz, music_update_pitch_registers_opl_2op
        music_update_pitch_registers_opl_2op_done:

        ; update volume if there's been a master volume change event
        ; or there's a volume macro or a volume change
        ld a, b
        and a, CHAN_EVENT_VOLUME_CHANGE | CHAN_EVENT_VOLUME_MACRO
        jp nz, music_volume_change_opl_2op
        music_volume_change_opl_2op_done:

        ; clear temporary events
        ld a, b
        and a, CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        ld (ix + channel.events), a

        muco2_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret

.endif
