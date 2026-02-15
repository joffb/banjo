
; banjo sound driver
; Joe Kennedy 2025

; jumps to code to handle commands
music_command_jump_table_opna_rhythm:

	jp mpnl_note_on_opna_rhythm
	jp mpnl_note_off_opna_rhythm
	jp mpnl_skip_2_byte_command
	jp mpnl_volume_change_opna_rhythm
	jp mpnl_skip_2_byte_command
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
	jp mpnl_note_release
