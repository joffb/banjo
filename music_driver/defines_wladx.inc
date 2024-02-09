.define NOTE_ON		0x0
.define NOTE_OFF		0x1
.define INSTRUMENT_CHANGE		0x2
.define FM_PATCH		0x3
.define FM_DRUM		0x4
.define SN_NOISE_MODE		0x5
.define PITCH_SLIDE_UP		0x6
.define PITCH_SLIDE_DOWN		0x7
.define ARPEGGIO		0x8
.define PORTAMENTO		0x9
.define LINE_WAIT		0xa
.define END_PATTERN_LINE		0xff

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

.define BANJO_MAGIC_BYTE		0xba

.define SLOT_2_BANK_CHANGE		0xffff

.STRUCT instrument
	volume_macro_len: db
	volume_macro_mode: db
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
	arpeggio_pos: db
	arpeggio: ds 2
	order_table_ptr: dw		; pointer to the current order
	pattern_ptr: dw		; pointer to the current pattern
	line_wait: db		; wait for this many lines
	volume_macro_len: db
	volume_macro_pos: db
	volume_macro_mode: db
	volume_macro_loop: db
	volume_macro_ptr: dw
	fm_patch_shifted: db
	fm_base_reg: db
	fm_drum_trigger: db
	fm_drum_volume_mask: db
.ENDST

.STRUCT music_state
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
	channel_types: ds 32
	subtic: db
	tic: db
	line: db
	order: db
	process_new_line: db
	noise_mode: db
.ENDST

