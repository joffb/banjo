
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_sn: 

    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn

    ret 

; ix: channel
; iy: music state
_banjo_update_channel_sn: 

    call music_update_channel_sn
    
    ret 


.ifndef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_sn: 

    ld bc, #music_command_jump_table_sn
    call music_process_new_line

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, b
    jr z, mucsn_done

        ; process volume macro
        bit CHAN_FLAG_BIT_VOLUME_MACRO, b
        call nz, music_update_volume_macro

        ; process arp macro
        bit CHAN_FLAG_BIT_ARP_MACRO, b
        call nz, music_update_arp_macro

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, b
        jr z, muc_sn_no_ex_macro
        
            call music_update_ex_macro

            ; was this a duty macro?
            bit MACRO_TYPE_BIT_DUTY, channel.ex_macro_type(ix)
            jr z, muc_sn_ex_macro_done

                ; update noise mode
                ld a, channel.ex_macro_val(ix)
                ld music_state.noise_mode(iy), a

        muc_sn_ex_macro_done: 
        muc_sn_no_ex_macro: 

        ; b now has events
        ld b, channel.events(ix)

        ; change of noise duty flagged?
        ; could be set by noise command or ex macro
        bit CHAN_EVENT_BIT_DUTY_CHANGED, b
        jr z, muc_sn_no_duty

            ; change noise mode here if we've changed to ch3 based pitch
            ld a, music_state.noise_mode(iy)
            bit 1, a
            jr z, muc_sn_no_duty

                ; bit 7 being set flags that the noise mode is different
                or a, a
                call m, music_update_noise_mode_sn_ch3_pitch

        muc_sn_no_duty: 

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_DUTY_CHANGED
        and a, b
        call nz, music_update_pitch_registers_sn

        ; .assert  CHAN_EVENT_VOLUME_CHANGE==STATE_FLAG_MASTER_VOLUME_CHANGE

        ; update volume if there's been a volume change event
        ; or the master volume changed
        ld a, b
        or a, music_state.flags(iy)
        and a, #CHAN_EVENT_VOLUME_CHANGE
        jp nz, music_volume_change_sn

        music_volume_change_sn_done: 

        ; clear events which only last one frame
		ld a, b
		and a, #CHAN_EVENT_PITCH_CHANGE_ALWAYS|CHAN_EVENT_MACRO_RELEASE|CHAN_EVENT_LEGATO|CHAN_EVENT_TIC_WAIT
        ld channel.events(ix), a

        mucsn_done: 
            
            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 

.endif 


.ifdef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_sn: 

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, music_state.flags(iy)
    jr z, muc_sn_no_process_new_line

        ld de, #music_command_jump_table_sn
        call music_process_new_line

    muc_sn_no_process_new_line: 

    ; get flags in b for later
    ld b, channel.flags(ix)

    ; check that the note is on
    bit CHAN_FLAG_BIT_NOTE_ON, b
    jr z, mucsn_done

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, channel.events(ix)

        ; check if we need to update the pitch of this channel
        ld a, #CHAN_EVENT_PITCH_CHANGED|CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_sn
        music_update_pitch_registers_sn_done: 

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_sn
        music_volume_change_sn_done: 

        ; clear events
        ld channel.events(ix), #0

        mucsn_done: 
            
            ; move on to next channel
            ld de, #_sizeof_channel
            add ix, de

            ret 

.endif 

.ifdef BANJO_ALF

	_banjo_sn_alf_wait: 

        ; disable io
        ld a, (_banjo_memory_control_value)
        set 2, a
        out (#SMS_MEMORY_CONTROL_PORT), a

        ; read from alf ready register
        ; and loop until the lower nibble is 0xf
        bsaw_loop: 

            in a, (#0xf3)
            cpl 
            and a, #0xf
            jr nz, bsaw_loop

        ; reenable io
        ld a, (_banjo_memory_control_value)
        out (#SMS_MEMORY_CONTROL_PORT), a

        ret 

.endif 
