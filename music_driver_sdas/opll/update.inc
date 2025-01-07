
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_opll: 

    push bc

    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll
    call music_update_channel_opll

    pop bc

    ret 

; ix: channel
; iy: music state
_banjo_update_channel_opll: 

    call music_update_channel_opll
    
    ret 

; ix: channel
; iy: music state
music_update_channel_opll: 

    ; note delay?
    ld a, channel.tic_wait(ix)
    or a, a
    jr z, muc_opll_no_note_delay

        ; decrement tic wait and check if it's zero
        dec a
        ld channel.tic_wait(ix), a
        jr nz, muc_opll_no_note_delay

            ; if it is, process the delayed line data
            ld de, #music_command_jump_table_opll
	        call music_process_new_line

    muc_opll_no_note_delay: 

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, music_state.flags(iy)
    jr z, muc_opll_no_process_new_line

        ld de, #music_command_jump_table_opll
        call music_process_new_line

    muc_opll_no_process_new_line: 

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that the note is on and the channel isn't muted
    ld a, #CHAN_FLAG_MUTED|CHAN_FLAG_NOTE_ON
    and a, b
    cp a, #CHAN_FLAG_NOTE_ON
    jr nz, muco_done

        ; process volume macro
        bit CHAN_FLAG_BIT_VOLUME_MACRO, b
        call nz, music_update_volume_macro

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, b
        call nz, music_update_ex_macro

        ; apply pitch slides/portamento
        bit CHAN_FLAG_BIT_SLIDE, b
        call nz, music_update_pitch_slide_opll

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, channel.events(ix)

        ; check for vibrato and arpeggio
        ; if either of these are active, the pitch changes every frame
        and a, #CHAN_FLAG_VIBRATO|CHAN_FLAG_ARPEGGIO
        jr z, music_update_opll_no_vib_arp

            ; if either are active, set pitch changed bit in events
            set CHAN_EVENT_BIT_PITCH_CHANGED, b

        music_update_opll_no_vib_arp: 

        ; wave/patch has been changed?
        bit CHAN_EVENT_BIT_WAVE_CHANGED, b
        jr z, muc_opll_no_wave_change

            set CHAN_EVENT_BIT_VOLUME_CHANGE, b

        muc_opll_no_wave_change: 

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_ARP_CHANGED|CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_opll
        music_update_pitch_registers_opll_done: 

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opll
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, music_state.flags(iy)
        jp nz, music_volume_change_opll
        music_volume_change_opll_done: 

        ; clear events
        ld channel.events(ix), #0

        muco_done: 

            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 

