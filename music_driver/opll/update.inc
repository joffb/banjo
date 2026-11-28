
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_opll:

    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll

    ret


; ix: channel
; iy: music state
banjo_update_channel_opll:

    call music_update_channel_opll
    
    ret


; delay after writing to opll chip
banjo_opll_write_delay:
    push hl
    pop hl
    push hl
    pop hl
    ret


.ifndef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_opll:

    ld hl, music_command_jump_table_opll
    call music_process_new_line

    ; handle note-off    
    bit CHAN_FLAG_BIT_NOTE_OFF, c
    jp nz, music_note_off_opll
    music_note_off_opll_done:

    ; store new value of flags
    ld (ix + channel.flags), c

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, c
    jr z, muco_done

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, c
        jr z, muco_no_ex_macro

            call music_update_ex_macro

            ; if this is the same patch number, don't do anything
            ld a, (hl)
            cp a, (ix + channel.gp1)
            jr z, muco_ex_macro_same_patch

                ; update patch number
                ; jump to volume change as patch shares a register with volume
                ld (ix + channel.gp1), a
                jp music_volume_change_opll
            
        muco_ex_macro_same_patch:
        muco_no_ex_macro:

        ; update volume if there's been a master volume change event
        ; or there's a volume macro or a volume change
        ld a, b
        and a, CHAN_EVENT_VOLUME_CHANGE | CHAN_EVENT_VOLUME_MACRO
        jp nz, music_volume_change_opll
        
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_opll      
        music_volume_change_opll_done:

        ; check if we need to update the pitch of this channel
        ld a, b
        and a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        jp nz, music_update_pitch_registers_opll
        music_update_pitch_registers_opll_done:

        ; clear temporary events
        ld a, b
        and a, CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        ld (ix + channel.events), a

        muco_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix, de

            ret



.endif


.ifdef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_opll:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    jr z, muc_opll_no_process_new_line

        ld de, music_command_jump_table_opll
        call music_process_new_line

    muc_opll_no_process_new_line:

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    jr nz, muco_done

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, (ix + channel.events)

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_WAVEDUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_opll
        music_update_pitch_registers_opll_done:

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opll
        music_volume_change_opll_done:

        ; clear events
        ld (ix + channel.events), 0

        muco_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret

.endif
