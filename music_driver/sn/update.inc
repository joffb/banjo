
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_sn:

    push bc

    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn

    pop bc

    ret

; ix: channel
; iy: music state
banjo_update_channel_sn:

    call music_update_channel_sn
    
    ret


.ifndef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_sn:

    ; note delay?
    ld a, (ix + channel.tic_wait)
    or a, a
    jr z, muc_sn_no_note_delay

        ; decrement tic wait and check if it's zero
        dec a
        ld (ix + channel.tic_wait), a
        jr nz, muc_sn_no_note_delay

            ; if it is, process the delayed line data
            ld bc, music_command_jump_table_sn
	        call music_process_new_line

    muc_sn_no_note_delay:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    jr z, muc_sn_no_process_new_line

        ld bc, music_command_jump_table_sn
        call music_process_new_line

    muc_sn_no_process_new_line:

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    jr nz, mucsn_done

        ; process volume macro
        bit CHAN_FLAG_BIT_VOLUME_MACRO, b
        call nz, music_update_volume_macro

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, b
        call nz, music_update_ex_macro

        ; apply pitch slides/portamento
        bit CHAN_FLAG_BIT_SLIDE, b
        call nz, music_update_pitch_slide_sn

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, (ix + channel.events)

        music_update_sn_no_vib_arp:

        ; change of noise duty?
        bit CHAN_EVENT_BIT_DUTY_CHANGED, b
        jr z, muc_sn_no_duty

            ; update noise mode
            ld a, (ix + channel.ex_macro_val)
            ld (iy + music_state.noise_mode), a

        muc_sn_no_duty:

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_PITCH_CHANGE_ALWAYS | CHAN_EVENT_ARP_CHANGED | CHAN_EVENT_DUTY_CHANGED
        and a, b 
        jp nz, music_update_pitch_registers_sn
        
        music_update_pitch_registers_sn_done:

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_sn

        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_sn        

        music_volume_change_sn_done:

        ; clear events
		ld a, b
		and a, ~(CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VOLUME_CHANGE)
        ld (ix + channel.events), a

        mucsn_done:
            
            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret

.endif


.ifdef BANJO_MINIMAL

; ix: channel
; iy: music state
music_update_channel_sn:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    jr z, muc_sn_no_process_new_line

        ld de, music_command_jump_table_sn
        call music_process_new_line

    muc_sn_no_process_new_line:

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    jr nz, mucsn_done

        ; keep flags in a
        ld a, b

        ; b now has events
        ld b, (ix + channel.events)

        ; check if we need to update the pitch of this channel
        ld a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_ARP_CHANGED | CHAN_EVENT_DUTY_CHANGED
        and a, b
        jp nz, music_update_pitch_registers_sn
        music_update_pitch_registers_sn_done:

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_sn
        music_volume_change_sn_done:

        ; clear events
        ld (ix + channel.events), 0

        mucsn_done:
            
            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret

.endif

.ifdef BANJO_ALF

	banjo_sn_alf_wait:

        ; disable io
        ld a, (banjo_memory_control_value)
        set 2, a
        out (SMS_MEMORY_CONTROL_PORT), a

        ; read from alf ready register
        ; and loop until the lower nibble is 0xf
        bsaw_loop:

            in a, (0xf3)
            cpl
            and a, 0xf
            jr nz, bsaw_loop

        ; reenable io
        ld a, (banjo_memory_control_value)
        out (SMS_MEMORY_CONTROL_PORT), a

        ret

.endif