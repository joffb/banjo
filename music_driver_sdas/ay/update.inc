
; banjo sound driver
; Joe Kennedy 2024

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_ay: 

    call music_update_channel_ay
    call music_update_channel_ay
    call music_update_channel_ay

    ret 


; ix: channel
; iy: music state
_banjo_update_channel_ay: 

    call music_update_channel_ay

    ret 


; ix: channel
; iy: music state
music_update_channel_ay: 

    ld bc, #music_command_jump_table_ay
    call music_process_new_line

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that the note is on and the channel isn't muted
    ld a, #CHAN_FLAG_MUTED|CHAN_FLAG_NOTE_ON
    and a, b
    cp a, #CHAN_FLAG_NOTE_ON
    jr nz, mucay_done

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

        ; change of noise duty?
        bit CHAN_EVENT_BIT_DUTY_CHANGED, b
        jr z, muc_ay_no_duty

            ; select noise reg
            ld a, #6
            out (#AY_REG_WRITE), a

            ; get noise amount and write it
            ld a, channel.ex_macro_val(ix)
            out (#AY_DATA_WRITE), a

        muc_ay_no_duty: 

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_PITCH_CHANGE_ALWAYS
        and a, b
        jp nz, music_update_pitch_registers_ay
        music_update_pitch_registers_ay_done: 

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_ay
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, music_state.flags(iy)
        jp nz, music_volume_change_ay
        music_volume_change_ay_done: 

        ; clear events
		ld a, b
		and a, #CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_MACRO_RELEASE|CHAN_EVENT_LEGATO|CHAN_EVENT_TIC_WAIT
        ld channel.events(ix), a
        
        mucay_done: 

            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de
            
            ret 

