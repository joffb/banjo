
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

    ; clear note-on/note-off bytes
    ld hl, #0
    ld (opll_drum_note_ons), hl

    ; update rhythm channels
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums
    call music_update_channel_opll_drums

    ; process note-on/note-off events
    ; note-offs in d, note-ons in e
    ld de, (opll_drum_note_ons)
    ld hl, #opll_drum_note_status

    ; writing note-offs to rhythm register
    ld a, #0xe
    out (#OPLL_REG_PORT), a

    ; any note-off events?
    ld a, d
    or a, a
    jr z, buc_opll_dr_no_note_offs

        ; invert bits to clear drum note-offs
        cpl 
        and a, (hl)
        ld (hl), a

        ; write to rhythm register
        out (#OPLL_DATA_PORT), a

        call _banjo_opll_write_delay

    buc_opll_dr_no_note_offs: 

    ; any note-on events?
    ld a, e
    or a, a
    jr z, buc_opll_dr_no_note_ons

        or a, (hl)
        ld (hl), a

        ; write to rhythm register
        out (#OPLL_DATA_PORT), a

    buc_opll_dr_no_note_ons: 

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

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, b
    jr z, mucod_done

        ld b, channel.events(ix)

        ; check if we need to update the pitch of this channel
        bit CHAN_EVENT_BIT_PITCH_CHANGED, b
        jp nz, music_update_pitch_registers_opll_drums
        music_update_pitch_registers_opll_drums_done: 

        ; update volume if there's been a volume change event
        ; or the master volume changed
        ld a, b
        or a, music_state.flags(iy)
        and a, #CHAN_EVENT_VOLUME_CHANGE
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


