
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_opll: 

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
_banjo_update_channel_opll: 

    call music_update_channel_opll
    
    ret 


; delay after writing to opll chip
_banjo_opll_write_delay: 
    push hl
    pop hl
    push hl
    pop hl
    ret 


.ifndef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_opll: 

    ld bc, #music_command_jump_table_opll
    call music_process_new_line

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, b
    jr z, muco_done

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
        ld b, channel.events(ix)

        ; wave/patch has been changed?
        bit CHAN_EVENT_BIT_WAVE_CHANGED, b
        jr z, muc_opll_no_wave_change

            set CHAN_EVENT_BIT_VOLUME_CHANGE, b

        muc_opll_no_wave_change: 

        ; update volume if there's been a volume change event
        ; or the master volume changed
        ld a, b
        or a, music_state.flags(iy)
        and a, #CHAN_EVENT_VOLUME_CHANGE
        jp nz, music_volume_change_opll
        music_volume_change_opll_done: 

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_opll
        music_update_pitch_registers_opll_done: 

        ; clear events
		ld a, b
		and a, #CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_MACRO_RELEASE|CHAN_EVENT_LEGATO|CHAN_EVENT_TIC_WAIT
        ld channel.events(ix), a

        muco_done: 

            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 



.endif 


.ifdef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_opll: 

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

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, channel.events(ix)

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_opll
        music_update_pitch_registers_opll_done: 

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opll
        music_volume_change_opll_done: 

        ; clear events
        ld channel.events(ix), #0

        muco_done: 

            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 

.endif 
