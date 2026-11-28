
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_sn:

    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn
    call music_update_channel_sn

    ret

; ix: channel
; iy: music state
banjo_update_channel_sn:

    call music_update_channel_sn
    
    ret


; ix: channel
; iy: music state
music_update_channel_sn:

    ; process new line if necessary
    ; returns events in b and flags in c
    ld hl, music_command_jump_table_sn
    call music_process_new_line

    ; handle note-off
    bit CHAN_FLAG_BIT_NOTE_OFF, c
    jp nz, music_note_off_sn
    music_note_off_sn_done:

    ; store new value of flags
    ld (ix + channel.flags), c

    ; check that note on is set
    ; this should never be set if the channel is muted
    bit CHAN_FLAG_BIT_NOTE_ON, c
    jr z, mucsn_done

        ; process ex macro
        bit CHAN_FLAG_BIT_EX_MACRO, c
        jr z, muc_sn_no_ex_macro
        
            call music_update_ex_macro

            ; update noise mode
            ld a, (hl)
            
            ; check if the noise mode has changed
            cp a, (iy + music_state.noise_mode)
            jr z, muc_sn_no_ex_macro_noise_change

                ; update value and flag a change
                ld (iy + music_state.noise_mode), a
                set CHAN_EVENT_BIT_WAVEDUTY_CHANGED, b

        muc_sn_no_ex_macro_noise_change:
        muc_sn_no_ex_macro:

        ; change of noise duty flagged?
        ; could be set by noise command or ex macro
        bit CHAN_EVENT_BIT_WAVEDUTY_CHANGED, b
        jr z, muc_sn_no_duty

            ; change noise mode here if we've changed to ch3 based pitch
            ld a, (iy + music_state.noise_mode)
            bit 1, a
            jr z, muc_sn_no_duty

                ; bit 7 being set flags that the noise mode is different
                call music_update_noise_mode_sn_ch3_pitch

        muc_sn_no_duty:

        ; check if we need to update the pitch of this channel
        ld a, b
        and a, CHAN_EVENT_PITCH_CHANGED | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_WAVEDUTY_CHANGED 
        call nz, music_update_pitch_registers_sn

        ; update volume if there's a volume macro or a volume change
        ; or there's been a master volume change event
        ld a, b
        and a, CHAN_EVENT_VOLUME_CHANGE | CHAN_EVENT_VOLUME_MACRO
        jp nz, music_volume_change_sn
        music_volume_change_sn_done:

        ; clear temporary events
        ld a, b
        and a, CHAN_EVENT_VOLUME_MACRO | CHAN_EVENT_ARP_MACRO | CHAN_EVENT_VIBRATO | CHAN_EVENT_ARPEGGIO | CHAN_EVENT_SLIDE
        ld (ix + channel.events), a

        mucsn_done:
            
            ; move on to next channel
            ld de, _sizeof_channel
            add ix,de

            ret




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