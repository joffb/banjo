
; banjo sound driver
; Joe Kennedy 2023

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_opll_drums:

    push bc

    ; update melodic channels

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    call music_update_channel_opll
    ld de, _sizeof_channel
    add ix,de

    ; update rhythm channels

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ld a, 0x16
    ld (opll_drum_pitch_register), a
    ld a, 0x10
    ld (opll_drum_trigger), a
    ld a, 0xf0
    ld (opll_drum_volume_mask), a

    ; update channel
    call music_update_channel_opll_drums
    ld de, _sizeof_channel
    add ix,de

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ld a, 0x17
    ld (opll_drum_pitch_register), a
    ld a, 0x8
    ld (opll_drum_trigger), a
    ld a, 0xf0
    ld (opll_drum_volume_mask), a

    ; update channel
    call music_update_channel_opll_drums
    ld de, _sizeof_channel
    add ix,de

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ld a, 0x18
    ld (opll_drum_pitch_register), a
    ld a, 0x4
    ld (opll_drum_trigger), a
    ld a, 0x0f
    ld (opll_drum_volume_mask), a

    ; update channel
    call music_update_channel_opll_drums
    ld de, _sizeof_channel
    add ix,de

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ld a, 0x18
    ld (opll_drum_pitch_register), a
    ld a, 0x2
    ld (opll_drum_trigger), a
    ld a, 0xf0
    ld (opll_drum_volume_mask), a

    ; update channel
    call music_update_channel_opll_drums
    ld de, _sizeof_channel
    add ix,de

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ld a, 0x17
    ld (opll_drum_pitch_register), a
    ld a, 0x1
    ld (opll_drum_trigger), a
    ld a, 0x0f
    ld (opll_drum_volume_mask), a

    ; update channel
    call music_update_channel_opll_drums
    ld de, _sizeof_channel
    add ix,de

    pop bc

    ret


; ix: channel
; iy: music state
banjo_update_channel_opll_drums:

    ; set opll_drum_pitch/trigger/volume variables for this channel
    ; get subchannel * 3 into a
    ld a, (ix + channel.subchannel)
    ld c, a
    add a, a
    add a, c

    ; index into table
    add a, <opll_drum_pitch_trigger_masks
    ld l, a
    adc a, >opll_drum_pitch_trigger_masks
    sub a, l
    ld h, a

    ; get values from table
    ld a, (hl)
    ld (opll_drum_pitch_register), a
    inc hl
    ld a, (hl)
    ld (opll_drum_trigger), a
    inc hl
    ld a, (hl)
    ld (opll_drum_volume_mask), a

    call music_update_channel_opll_drums
    
    ret


; ix: channel
; iy: music state
music_update_channel_opll_drums:

    ; note delay?
    ld a, (ix + channel.tic_wait)
    or a, a
    jr z, muc_opll_drums_no_note_delay

        ; decrement tic wait and check if it's zero
        dec a
        ld (ix + channel.tic_wait), a
        jr nz, muc_opll_drums_no_note_delay

            ; if it is, process the delayed line data
            ld de, music_command_jump_table_opll_drums
	        call music_process_new_line

    muc_opll_drums_no_note_delay:

    ; process new line if necessary
	bit STATE_FLAG_BIT_PROCESS_NEW_LINE, (iy + music_state.flags)
    jr z, muc_opll_drums_no_process_new_line

        ld de, music_command_jump_table_opll_drums
        call music_process_new_line

    muc_opll_drums_no_process_new_line:

    ; get flags in b for later
    ld b, (ix + channel.flags)

    ; check that the note is on and the channel isn't muted
    ld a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_ON
    and a, b
    cp a, CHAN_FLAG_NOTE_ON
    ret nz

        ; update volume if there's been a volume change event
        bit CHAN_EVENT_BIT_VOLUME_CHANGE, b
        jp nz, music_volume_change_opll_drums
        ; if not, has the master volume changed?
        bit STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)
        jp nz, music_volume_change_opll_drums
        music_volume_change_opll_drums_done:

        ; clear events
        ld (ix + channel.events), 0

        ; move on to next channel
        ret


; jumps to code to handle commands
music_command_jump_table_opll_drums:

	jp mpnl_note_on_opll_drums
	jp mpnl_note_off_opll_drums
	jp mpnl_instrument_change
	jp mpnl_volume_change
	jp mpnl_opll_drum
	jp mpnl_skip_2_byte_command     ; mpnl_sn_noise_mode
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
	jp mpnl_skip_3_byte_command     ; mpnl_game_gear_pan
    jp mpnl_skip_1_byte_command     ; mpnl_ay_env_on
	jp mpnl_skip_1_byte_command     ; mpnl_ay_env_off
    jp mpnl_skip_2_byte_command     ; mpnl_ay_env_shape
    jp mpnl_skip_2_byte_command     ; mpnl_ay_env_period_hi
	jp mpnl_skip_2_byte_command     ; mpnl_ay_env_period_lo
	jp mpnl_skip_2_byte_command     ; mpnl_ay_channel_mix
	jp mpnl_skip_2_byte_command     ; mpnl_ay_noise_pitch
	jp mpnl_skip_3_byte_command     ; mpnl_ay_env_period_word
	jp mpnl_order_jump
	jp mpnl_set_speed_1
	jp mpnl_set_speed_2
    jp mpnl_order_next
    jp mpnl_note_delay