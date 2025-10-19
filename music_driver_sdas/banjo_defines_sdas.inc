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
AY_ENV_ON			.equ		0x11
AY_ENV_OFF			.equ		0x12
AY_ENV_SHAPE			.equ		0x13
AY_ENV_PERIOD_HI			.equ		0x14
AY_ENV_PERIOD_LO			.equ		0x15
AY_CHANNEL_MIX			.equ		0x16
AY_NOISE_PITCH			.equ		0x17
AY_ENV_PERIOD_WORD			.equ		0x18
ORDER_JUMP			.equ		0x19
SET_SPEED_1			.equ		0x1a
SET_SPEED_2			.equ		0x1b
ORDER_NEXT			.equ		0x1c
NOTE_DELAY			.equ		0x1d
END_LINE			.equ		0x80

GAME_GEAR_PORT_0			.equ		0x0
GAME_GEAR_PAN_PORT			.equ		0x6
SMS_MEMORY_CONTROL_PORT			.equ		0x3e
SMS_AUDIO_CONTROL_PORT			.equ		0xf2
BANJO_RDSLT			.equ		0xc
BANJO_WRTSLT			.equ		0x14
MSX_FMPAC_IO_ENABLE			.equ		0x7ff6
MSX_FMPAC_STRING_LOC			.equ		0x4018

SLIDE_TYPE_NONE			.equ		0x0
SLIDE_TYPE_UP			.equ		0x1
SLIDE_TYPE_DOWN			.equ		0x2
SLIDE_TYPE_PORTA			.equ		0x3

CHAN_SN76489			.equ		0x0
CHAN_OPLL			.equ		0x1
CHAN_OPLL_DRUMS			.equ		0x2
CHAN_AY_3_8910			.equ		0x3

CHAN_COUNT_SN			.equ		0x4
CHAN_COUNT_OPLL			.equ		0x9
CHAN_COUNT_OPLL_DRUMS			.equ		0xb
CHAN_COUNT_AY			.equ		0x3

CHAN_FLAG_MUTED			.equ		0x1
CHAN_FLAG_NOTE_ON			.equ		0x2
CHAN_FLAG_LEGATO			.equ		0x4
CHAN_FLAG_VOLUME_MACRO			.equ		0x8
CHAN_FLAG_VIBRATO			.equ		0x10
CHAN_FLAG_ARPEGGIO			.equ		0x20
CHAN_FLAG_SLIDE			.equ		0x40
CHAN_FLAG_EX_MACRO			.equ		0x80

CHAN_FLAG_BIT_MUTED			.equ		0x0
CHAN_FLAG_BIT_NOTE_ON			.equ		0x1
CHAN_FLAG_BIT_LEGATO			.equ		0x2
CHAN_FLAG_BIT_VOLUME_MACRO			.equ		0x3
CHAN_FLAG_BIT_VIBRATO			.equ		0x4
CHAN_FLAG_BIT_ARPEGGIO			.equ		0x5
CHAN_FLAG_BIT_SLIDE			.equ		0x6
CHAN_FLAG_BIT_EX_MACRO			.equ		0x7

CHAN_EVENT_PITCH_CHANGED			.equ		0x1
CHAN_EVENT_VOLUME_CHANGE			.equ		0x2
CHAN_EVENT_PITCH_CHANGE_ALWAYS			.equ		0x4
CHAN_EVENT_VOLUME_CHANGE_ALWAYS			.equ		0x8
CHAN_EVENT_WAVE_CHANGED			.equ		0x10
CHAN_EVENT_DUTY_CHANGED			.equ		0x20
CHAN_EVENT_ARP_CHANGED			.equ		0x40
CHAN_EVENT_MACRO_RELEASE			.equ		0x80

CHAN_EVENT_BIT_PITCH_CHANGED			.equ		0x0
CHAN_EVENT_BIT_VOLUME_CHANGE			.equ		0x1
CHAN_EVENT_BIT_PITCH_CHANGE_ALWAYS			.equ		0x2
CHAN_EVENT_BIT_VOLUME_CHANGE_ALWAYS			.equ		0x3
CHAN_EVENT_BIT_WAVE_CHANGED			.equ		0x4
CHAN_EVENT_BIT_DUTY_CHANGED			.equ		0x5
CHAN_EVENT_BIT_ARP_CHANGED			.equ		0x6
CHAN_EVENT_BIT_MACRO_RELEASE			.equ		0x7

MACRO_TYPE_PITCH			.equ		0x1
MACRO_TYPE_VOLUME			.equ		0x2
MACRO_TYPE_WAVE			.equ		0x10
MACRO_TYPE_DUTY			.equ		0x20
MACRO_TYPE_ARP			.equ		0x40

MACRO_TYPE_BIT_PITCH			.equ		0x0
MACRO_TYPE_BIT_VOLUME			.equ		0x1
MACRO_TYPE_BIT_WAVE			.equ		0x4
MACRO_TYPE_BIT_DUTY			.equ		0x5
MACRO_TYPE_BIT_ARP			.equ		0x6

STATE_FLAG_PROCESS_NEW_LINE			.equ		0x1
STATE_FLAG_LOOP			.equ		0x2
STATE_FLAG_MASTER_VOLUME_CHANGE			.equ		0x4

STATE_FLAG_BIT_PROCESS_NEW_LINE			.equ		0x0
STATE_FLAG_BIT_LOOP			.equ		0x1
STATE_FLAG_BIT_MASTER_VOLUME_CHANGE			.equ		0x2

BANJO_MAGIC_BYTE			.equ		0xba

SLOT_2_BANK_CHANGE			.equ		0xffff

BANJO_HAS_SN			.equ		0x1
BANJO_HAS_OPLL			.equ		0x2
BANJO_HAS_AY			.equ		0x4
BANJO_HAS_DUAL_SN			.equ		0x8

BANJO_HAS_BIT_SN			.equ		0x0
BANJO_HAS_BIT_OPLL			.equ		0x1
BANJO_HAS_BIT_AY			.equ		0x2
BANJO_HAS_BIT_DUAL_SN			.equ		0x3

BANJO_LOOP_OFF			.equ		0x0
BANJO_LOOP_ON			.equ		0x1

SYS_FLAG_GG			.equ		0x1
SYS_FLAG_MARK_III			.equ		0x2
SYS_FLAG_SYSTEM_E			.equ		0x4

SYS_FLAG_BIT_GG			.equ		0x0
SYS_FLAG_BIT_MARK_III			.equ		0x1
SYS_FLAG_BIT_SYSTEM_E			.equ		0x2

instrument.macro_flags .equ 0
instrument.volume_macro_ptr .equ 1
instrument.ex_macro_type .equ 3
instrument.ex_macro_ptr .equ 4
instrument.fm_preset .equ 6
instrument.fm_patch .equ 7
instrument.padding .equ 9
_sizeof_instrument .equ 10

channel.flags .equ 0
channel.subchannel .equ 1		; channel number within the chip
channel.port .equ 2		; output port, used for certain chips
channel.events .equ 3		; 
channel.freq .equ 4		; current fnum/tone of the voice
channel.target_freq .equ 6		; target fnum/tone used for portamento
channel.volume .equ 8
channel.midi_note .equ 9
channel.instrument_num .equ 10
channel.slide_amount .equ 11		; how much to add/subtract per tic
channel.slide_type .equ 12		; type of slide (up/down/portamento)
channel.vibrato_depth .equ 13
channel.vibrato_counter .equ 14
channel.vibrato_counter_add .equ 15
channel.arpeggio_pos .equ 16
channel.arpeggio .equ 17
channel.pattern_ptr .equ 18		; pointer to the current pattern
channel.line_wait .equ 20		; wait for this many lines
channel.tic_wait .equ 21		; wait for this many tics
channel.volume_macro_vol .equ 22
channel.volume_macro_pos .equ 23
channel.volume_macro_ptr .equ 24
channel.ex_macro_val .equ 26
channel.ex_macro_pos .equ 27
channel.ex_macro_type .equ 28
channel.ex_macro_ptr .equ 29
channel.patch .equ 31
_sizeof_channel .equ 32

music_state.magic_byte .equ 0		; should be 0xba for a valid song
music_state.bank .equ 1
music_state.channel_count .equ 2
music_state.flags .equ 3
music_state.master_volume .equ 4
music_state.master_volume_fade .equ 5
music_state.has_chips .equ 6
music_state.sfx_channel .equ 7
music_state.sfx_subchannel .equ 8
music_state.time_base .equ 9
music_state.speed_1 .equ 10
music_state.speed_2 .equ 11
music_state.pattern_length .equ 12
music_state.orders_length .equ 13
music_state.instrument_ptrs .equ 14
music_state.order_ptrs .equ 16
music_state.tic .equ 18
music_state.line .equ 19
music_state.order .equ 20
music_state.order_jump .equ 21
music_state.noise_mode .equ 22
music_state.panning .equ 23
music_state.channel_init_call .equ 24
music_state.channel_update_call .equ 26
music_state.channel_mute_calls .equ 28
music_state.song_mute .equ 30
music_state.song_stop .equ 32
_sizeof_music_state .equ 34

