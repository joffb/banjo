
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_opll_drums: 

    ; update melodic channels
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll

    ; update rhythm channels
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums

    ret 


; ix: channel
; iy: music state
_banjo_update_channel_opll_drums: 

    call music_update_channel_opll_drums
    
    ret 


; ix: channel
; iy: music state
music_update_channel_opll_drums: 

    ld bc, #music_command_jump_table_opll_drums
    call music_process_new_line

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that the note is on and the channel isn't muted
    ld a, #CHAN_FLAG_MUTED|CHAN_FLAG_NOTE_ON
    and a, b
    cp a, #CHAN_FLAG_NOTE_ON
    jr nz, mucod_done

        ld b, channel.events(ix)

        ; check if we need to update the pitch of this channel
        bit CHAN_EVENT_BIT_PITCH_CHANGED, b
        jp nz, music_update_pitch_registers_opll_drums
        music_update_pitch_registers_opll_drums_done: 

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opll_drums
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, music_state.flags(iy)
        jp nz, music_volume_change_opll_drums
        music_volume_change_opll_drums_done: 

        ; clear events
		ld a, b
		and a, #CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_MACRO_RELEASE|CHAN_EVENT_LEGATO|CHAN_EVENT_TIC_WAIT
        ld channel.events(ix), a

        mucod_done: 

            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 


