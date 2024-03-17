NOTE_ON			.equ		0x0
NOTE_OFF			.equ		0x1
INSTRUMENT_CHANGE			.equ		0x2
VOLUME_CHANGE			.equ		0x3
FM_DRUM			.equ		0x4
SN_NOISE_MODE			.equ		0x5
SLIDE_UP			.equ		0x6
SLIDE_DOWN			.equ		0x7
SLIDE_PORTA			.equ		0x8
SLIDE_OFF			.equ		0x9
ARPEGGIO			.equ		0xa
ARPEGGIO_OFF			.equ		0xb
VIBRATO			.equ		0xc
VIBRATO_OFF			.equ		0xd
LEGATO_ON			.equ		0xe
LEGATO_OFF			.equ		0xf
GAME_GEAR_PAN			.equ		0x10
END_LINE			.equ		0x80

GAME_GEAR_PORT_0			.equ		0x0
GAME_GEAR_PAN_PORT			.equ		0x6
SN76489_PORT			.equ		0x7f
SN76489_2_PORT			.equ		0x7b
OPLL_REG_PORT			.equ		0xf0
OPLL_DATA_PORT			.equ		0xf1
AUDIO_CONTROL_PORT			.equ		0xf2

CHAN_SN76489			.equ		0x0
CHAN_OPLL			.equ		0x1
CHAN_OPLL_DRUMS			.equ		0x2

SLIDE_TYPE_NONE			.equ		0x0
SLIDE_TYPE_UP			.equ		0x1
SLIDE_TYPE_DOWN			.equ		0x2
SLIDE_TYPE_PORTA			.equ		0x3

CHAN_FLAG_MUTED			.equ		0x1
CHAN_FLAG_NOTE_ON			.equ		0x2
CHAN_FLAG_LEGATO			.equ		0x4
CHAN_FLAG_PITCH_CHANGED			.equ		0x8
CHAN_FLAG_VOLUME_MACRO			.equ		0x10
CHAN_FLAG_VIBRATO			.equ		0x20
CHAN_FLAG_ARPEGGIO			.equ		0x40
CHAN_FLAG_SLIDE			.equ		0x80

CHAN_FLAG_BIT_MUTED			.equ		0x0
CHAN_FLAG_BIT_NOTE_ON			.equ		0x1
CHAN_FLAG_BIT_LEGATO			.equ		0x2
CHAN_FLAG_BIT_PITCH_CHANGED			.equ		0x3
CHAN_FLAG_BIT_VOLUME_MACRO			.equ		0x4
CHAN_FLAG_BIT_VIBRATO			.equ		0x5
CHAN_FLAG_BIT_ARPEGGIO			.equ		0x6
CHAN_FLAG_BIT_SLIDE			.equ		0x7

BANJO_MAGIC_BYTE			.equ		0xba

SLOT_2_BANK_CHANGE			.equ		0xffff

MODE_FM			.equ		0x1
MODE_SN			.equ		0x2
MODE_SN_FM			.equ		0x3
MODE_FM_DRUMS			.equ		0x5
MODE_DUAL_SN			.equ		0x6
MODE_SN_FM_DRUMS			.equ		0x7

VIBRATO_CENTRE			.equ		0x40

instrument.volume_macro_len .equ 0
instrument.volume_macro_loop .equ 1
instrument.volume_macro_ptr .equ 2
instrument.fm_preset .equ 4
instrument.fm_patch .equ 5
instrument.padding .equ 13
_sizeof_instrument .equ 15

channel.flags .equ 0		; 1: muted - 2: note on - 4: legato - 8: pitch changed
channel.type .equ 1		; type of chip
channel.subchannel .equ 2		; channel number within the chip
channel.port .equ 3		; output port for multiples of SN chip
channel.freq .equ 4		; current fnum/tone of the voice
channel.target_freq .equ 6		; target fnum/tone used for portamento
channel.volume .equ 8
channel.midi_note .equ 9
channel.instrument_num .equ 10
channel.slide_amount .equ 11		; how much to add/subtract per tic
channel.slide_type .equ 12		; type of slide (up/down/portamento)
channel.vibrato_current .equ 13
channel.vibrato_target .equ 14
channel.vibrato_counter .equ 15
channel.vibrato_counter_add .equ 16
channel.arpeggio_pos .equ 17
channel.arpeggio .equ 18
channel.order_table_ptr .equ 19		; pointer to the current order
channel.pattern_ptr .equ 21		; pointer to the current pattern
channel.line_wait .equ 23		; wait for this many lines
channel.volume_macro_len .equ 24
channel.volume_macro_pos .equ 25
channel.volume_macro_loop .equ 26
channel.volume_macro_ptr .equ 27
channel.fm_patch_shifted .equ 29
channel.fm_drum_trigger .equ 30
channel.fm_drum_volume_mask .equ 31
_sizeof_channel .equ 32

music_state.magic_byte .equ 0
music_state.bank .equ 1
music_state.channel_count .equ 2
music_state.loop .equ 3
music_state.sfx_channel .equ 4
music_state.has_sn .equ 5
music_state.has_fm .equ 6
music_state.has_fm_drums .equ 7
music_state.time_base .equ 8
music_state.speed_1 .equ 9
music_state.speed_2 .equ 10
music_state.pattern_length .equ 11
music_state.orders_length .equ 12
music_state.instrument_ptrs .equ 13
music_state.order_ptrs .equ 15
music_state.subtic .equ 17
music_state.tic .equ 18
music_state.line .equ 19
music_state.order .equ 20
music_state.process_new_line .equ 21
music_state.noise_mode .equ 22
music_state.panning .equ 23
music_state.channel_types .equ 24
_sizeof_music_state .equ 56

