
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

    ld bc, music_command_jump_table_opl_2op
    call music_process_new_line

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    jr nz, muco2_done

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

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_PITCH_CHANGE_ALWAYS | CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_opl_2op
        music_update_pitch_registers_opl_2op_done:

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opl_2op
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_opl_2op      
        music_volume_change_opl_2op_done:

        ; clear events
		ld a, b
		and a, CHAN_EVENT_PITCH_CHANGE_ALWAYS | CHAN_EVENT_MACRO_RELEASE | CHAN_EVENT_LEGATO | CHAN_EVENT_TIC_WAIT
        ld (ix + channel.events), a

        muco2_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret

.endif
