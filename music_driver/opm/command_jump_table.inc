
; banjo sound driver
; Joe Kennedy 2025

; jumps to code to handle commands
music_command_jump_table_opm:

	.dw mpnl_note_on_opm
	.dw mpnl_note_off
	.dw mpnl_opm_instrument_change
	.dw mpnl_opm_volume_change
	.dw mpnl_skip_2_byte_command		; mpnl_opll_drum
	.dw mpnl_skip_2_byte_command     ; mpnl_sn_noise_mode
	.dw mpnl_pitch_slide_up
	.dw mpnl_pitch_slide_down
	.dw mpnl_portamento
	.dw mpnl_slide_off
	.dw mpnl_arpeggio
	.dw mpnl_arpeggio_off
	.dw mpnl_vibrato
	.dw mpnl_vibrato_off
	.dw mpnl_legato_on
	.dw mpnl_legato_off
	.dw mpnl_opm_pan
    .dw mpnl_skip_1_byte_command     ; mpnl_ay_env_on
	.dw mpnl_skip_1_byte_command     ; mpnl_ay_env_off
    .dw mpnl_skip_2_byte_command     ; mpnl_ay_env_shape
    .dw mpnl_skip_2_byte_command     ; mpnl_ay_env_period_hi
	.dw mpnl_skip_2_byte_command     ; mpnl_ay_env_period_lo
	.dw mpnl_opm_noise
	.dw mpnl_opm_lfo_rate
	.dw mpnl_opm_lfo_wave
	.dw mpnl_order_jump
	.dw mpnl_set_speed_1
	.dw mpnl_set_speed_2
    .dw mpnl_order_next
    .dw mpnl_note_delay
	.dw mpnl_note_release
