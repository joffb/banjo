
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
_banjo_update_channels_opll: 

    push bc

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

    call music_update_channel_opll
    ld de, #_sizeof_channel
    add ix, de

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
    ret nz

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

        ; move on to next channel
        ret 

; jumps to code to handle commands
music_command_jump_table_opll: 

	jp mpnl_note_on_opll
	jp mpnl_note_off_opll
	jp mpnl_opll_instrument_change
	jp mpnl_volume_change
	jp mpnl_skip_2_byte_command; mpnl_opll_drum
	jp mpnl_skip_2_byte_command; mpnl_sn_noise_mode
	jp mpnl_pitch_slide_up
	jp mpnl_pitch_slide_down
	jp mpnl_portamento
	jp mpnl_slide_off
	jp mpnl_arpeggio
	jp mpnl_arpeggio_off
	jp mpnl_vibrato
	jp mpnl_vibrato_off
	jp mpnl_legato_on
	jp mpnl_legato_off
	jp mpnl_skip_3_byte_command; mpnl_game_gear_pan
    jp mpnl_skip_1_byte_command; mpnl_ay_env_on
	jp mpnl_skip_1_byte_command; mpnl_ay_env_off
    jp mpnl_skip_2_byte_command; mpnl_ay_env_shape
    jp mpnl_skip_2_byte_command; mpnl_ay_env_period_hi
	jp mpnl_skip_2_byte_command; mpnl_ay_env_period_lo
	jp mpnl_skip_2_byte_command; mpnl_ay_channel_mix
	jp mpnl_skip_2_byte_command; mpnl_ay_noise_pitch
	jp mpnl_skip_3_byte_command; mpnl_ay_env_period_word
	jp mpnl_order_jump
	jp mpnl_set_speed_1
	jp mpnl_set_speed_2
    jp mpnl_order_next
    jp mpnl_note_delay

