.define NOTE_ON		0x0
.define NOTE_OFF		0x1
.define INSTRUMENT_CHANGE		0x2
.define VOLUME_CHANGE		0x3
.define FM_DRUM		0x4
.define SN_NOISE_MODE		0x5
.define SLIDE_UP		0x6
.define SLIDE_DOWN		0x7
.define SLIDE_PORTA		0x8
.define SLIDE_OFF		0x9
.define ARPEGGIO		0xa
.define ARPEGGIO_OFF		0xb
.define VIBRATO		0xc
.define VIBRATO_OFF		0xd
.define LEGATO_ON		0xe
.define LEGATO_OFF		0xf
.define GAME_GEAR_PAN		0x10
.define END_LINE		0x80

.define GAME_GEAR_PORT_0		0x0
.define GAME_GEAR_PAN_PORT		0x6
.define SN76489_PORT		0x7f
.define SN76489_2_PORT		0x7b
.define OPLL_REG_PORT		0xf0
.define OPLL_DATA_PORT		0xf1
.define AUDIO_CONTROL_PORT		0xf2

.define CHAN_SN76489		0x0
.define CHAN_OPLL		0x1
.define CHAN_OPLL_DRUMS		0x2

.define SLIDE_TYPE_NONE		0x0
.define SLIDE_TYPE_UP		0x1
.define SLIDE_TYPE_DOWN		0x2
.define SLIDE_TYPE_PORTA		0x3

.define CHAN_FLAG_MUTED		0x1
.define CHAN_FLAG_NOTE_ON		0x2
.define CHAN_FLAG_LEGATO		0x4
.define CHAN_FLAG_PITCH_CHANGED		0x8
.define CHAN_FLAG_VOLUME_MACRO		0x10
.define CHAN_FLAG_VIBRATO		0x20
.define CHAN_FLAG_ARPEGGIO		0x40
.define CHAN_FLAG_SLIDE		0x80

.define CHAN_FLAG_BIT_MUTED		0x0
.define CHAN_FLAG_BIT_NOTE_ON		0x1
.define CHAN_FLAG_BIT_LEGATO		0x2
.define CHAN_FLAG_BIT_PITCH_CHANGED		0x3
.define CHAN_FLAG_BIT_VOLUME_MACRO		0x4
.define CHAN_FLAG_BIT_VIBRATO		0x5
.define CHAN_FLAG_BIT_ARPEGGIO		0x6
.define CHAN_FLAG_BIT_SLIDE		0x7

.define BANJO_MAGIC_BYTE		0xba

.define SLOT_2_BANK_CHANGE		0xffff

.define MODE_FM		0x1
.define MODE_SN		0x2
.define MODE_SN_FM		0x3
.define MODE_FM_DRUMS		0x5
.define MODE_DUAL_SN		0x6
.define MODE_SN_FM_DRUMS		0x7

.define VIBRATO_CENTRE		0x40

.STRUCT instrument
	volume_macro_len: db
	volume_macro_loop: db
	volume_macro_ptr: dw
	fm_preset: db
	fm_patch: ds 8
	padding: ds 2
.ENDST

.STRUCT channel
	flags: db		; 1: muted - 2: note on - 4: legato - 8: pitch changed
	type: db		; type of chip
	subchannel: db		; channel number within the chip
	port: db		; output port for multiples of SN chip
	freq: dw		; current fnum/tone of the voice
	target_freq: dw		; target fnum/tone used for portamento
	volume: db
	midi_note: db
	instrument_num: db
	slide_amount: db		; how much to add/subtract per tic
	slide_type: db		; type of slide (up/down/portamento)
	vibrato_current: db
	vibrato_target: db
	vibrato_counter: db
	vibrato_counter_add: db
	arpeggio_pos: db
	arpeggio: db
	order_table_ptr: dw		; pointer to the current order
	pattern_ptr: dw		; pointer to the current pattern
	line_wait: db		; wait for this many lines
	volume_macro_len: db
	volume_macro_pos: db
	volume_macro_loop: db
	volume_macro_ptr: dw
	fm_patch_shifted: db
	fm_drum_trigger: db
	fm_drum_volume_mask: db
.ENDST

.STRUCT music_state
	magic_byte: db
	bank: db
	channel_count: db
	loop: db
	sfx_channel: db
	has_sn: db
	has_fm: db
	has_fm_drums: db
	time_base: db
	speed_1: db
	speed_2: db
	pattern_length: db
	orders_length: db
	instrument_ptrs: dw
	order_ptrs: dw
	subtic: db
	tic: db
	line: db
	order: db
	process_new_line: db
	noise_mode: db
	panning: db
	channel_types: ds 32
.ENDST

