NOTE_ON			.equ		0x0
NOTE_OFF			.equ		0x1
INSTRUMENT_CHANGE			.equ		0x2
FM_PATCH			.equ		0x3
FM_DRUM			.equ		0x4
SN_NOISE_MODE			.equ		0x5
PITCH_SLIDE_UP			.equ		0x6
PITCH_SLIDE_DOWN			.equ		0x7
ARPEGGIO			.equ		0x8
PORTAMENTO			.equ		0x9
LINE_WAIT			.equ		0xa
END_PATTERN_LINE			.equ		0xff

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

BANJO_MAGIC_BYTE			.equ		0xba

SLOT_2_BANK_CHANGE			.equ		0xffff

MODE_FM			.equ		0x1
MODE_SN			.equ		0x2
MODE_SN_FM			.equ		0x3
MODE_DUAL_SN			.equ		0x4
MODE_FM_DRUMS			.equ		0x5
MODE_SN_FM_DRUMS			.equ		0x7

instrument.volume_macro_len .equ 0
instrument.volume_macro_mode .equ 1
instrument.volume_macro_loop .equ 2
instrument.volume_macro_ptr .equ 3
instrument.fm_preset .equ 5
instrument.fm_patch .equ 6
instrument.padding .equ 14
_sizeof_instrument .equ 16

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
channel.arpeggio_pos .equ 13
channel.arpeggio .equ 14
channel.order_table_ptr .equ 16		; pointer to the current order
channel.pattern_ptr .equ 18		; pointer to the current pattern
channel.line_wait .equ 20		; wait for this many lines
channel.volume_macro_len .equ 21
channel.volume_macro_pos .equ 22
channel.volume_macro_mode .equ 23
channel.volume_macro_loop .equ 24
channel.volume_macro_ptr .equ 25
channel.fm_patch_shifted .equ 27
channel.fm_base_reg .equ 28
channel.fm_drum_trigger .equ 29
channel.fm_drum_volume_mask .equ 30
_sizeof_channel .equ 31

music_state.bank .equ 0
music_state.channel_count .equ 1
music_state.loop .equ 2
music_state.sfx_channel .equ 3
music_state.has_sn .equ 4
music_state.has_fm .equ 5
music_state.has_fm_drums .equ 6
music_state.time_base .equ 7
music_state.speed_1 .equ 8
music_state.speed_2 .equ 9
music_state.pattern_length .equ 10
music_state.orders_length .equ 11
music_state.instrument_ptrs .equ 12
music_state.order_ptrs .equ 14
music_state.channel_types .equ 16
music_state.subtic .equ 48
music_state.tic .equ 49
music_state.line .equ 50
music_state.order .equ 51
music_state.process_new_line .equ 52
music_state.noise_mode .equ 53
_sizeof_music_state .equ 54

