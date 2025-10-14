
; banjo sound driver
; Joe Kennedy 2024

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_ay:

    push bc

    call music_update_channel_ay
    call music_update_channel_ay
    call music_update_channel_ay

    pop bc

    ret


; ix: channel
; iy: music state
banjo_update_channel_ay:

    call music_update_channel_ay

    ret


; ix: channel
; iy: music state
music_update_channel_ay:

    ; note delay?
    ld a, (ix + channel.tic_wait)
    or a, a
    jr z, muc_ay_no_note_delay

        ; decrement tic wait and check if it's zero
        dec a
        ld (ix + channel.tic_wait), a
        jr nz, muc_ay_no_note_delay

            ; if it is, process the delayed line data
            ld bc, music_command_jump_table_ay
	        call music_process_new_line

    muc_ay_no_note_delay:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    jr z, muc_sn_no_process_new_line

        ld bc, music_command_jump_table_ay
        call music_process_new_line

    muc_sn_no_process_new_line:

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    jr nz, mucay_done

        ; process volume macro
        bit CHAN_FLAG_BIT_VOLUME_MACRO, b
        call nz, music_update_volume_macro

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, b
        call nz, music_update_ex_macro

        ; apply pitch slides/portamento
        bit CHAN_FLAG_BIT_SLIDE, b
        call nz, music_update_pitch_slide_ay

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, (ix + channel.events)

        ; change of noise duty?
        bit CHAN_EVENT_BIT_DUTY_CHANGED, b
        jr z, muc_ay_no_duty

            ; select noise reg
            ld a, 6
            out (AY_REG_WRITE), a

            ; get noise amount and write it
            ld a, (ix + channel.ex_macro_val)
            out (AY_DATA_WRITE), a

        muc_ay_no_duty:

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_PITCH_CHANGE_ALWAYS | CHAN_EVENT_ARP_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_ay
        music_update_pitch_registers_ay_done:

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_ay
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_ay      
        music_volume_change_ay_done:

        ; clear events
		ld a, b
		and a, CHAN_EVENT_PITCH_CHANGE_ALWAYS
        ld (ix + channel.events), a
        
        mucay_done:

            ; move on to next channel
            ld de, _sizeof_channel
            add ix, de
            
            ret

