
.include "../../music_driver/banjo_defines_wladx.inc"

.define VDP_CONTROL_PORT 0xbf
.define VDP_DATA_PORT 0xbe

.define VDP_WRITE_ADDRESS 0x4000
.define VDP_WRITE_CRAM 0xc000
.define VDP_WRITE_REGISTER 0x8000

.define CHAN_COUNT CHAN_COUNT_OPLL_DRUMS + CHAN_COUNT_SN

.MEMORYMAP
	SLOTSIZE $4000
	DEFAULTSLOT 0
	SLOT 0 $0000			; ROM slot 0.
	SLOT 1 $4000			; ROM slot 1.
	SLOT 2 $8000			; ROM slot 2
	SLOT 3 $C000			; RAM
.ENDME

.ROMBANKMAP
	BANKSTOTAL 4
	BANKSIZE $4000
	BANKS 4
.ENDRO

.RAMSECTION "Main Vars" bank 0 slot 3

	tic: db
	input_last: db
	input_pressed: db

	volume_control: db

	song_channels: INSTANCEOF channel (CHAN_COUNT)

.ENDS

.org 0x0000
	jp init

.org 0x0038

	vblank_interrupt:

	push af
	exx
	
	ld c, VDP_CONTROL_PORT
	
	; acknowledge interrupt
	in a, (c)
	
	exx
	pop af
	
	ei
	reti

.org 0x0066
	retn
	

init:
	di
	im 1

	ld sp, 0xdfff

	ld a, 1
	ld (0xfffe), a

	ld a, 2
	ld (0xffff), a

	; set vdp mode 4
	ld hl, VDP_WRITE_REGISTER | (0 << 8) | 0b00000100
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; disable vdp screens for now
	ld hl, VDP_WRITE_REGISTER | (1 << 8) | 0b10000000
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h
	
	; set vdp nametable base addresses
	ld hl, VDP_WRITE_REGISTER | (2 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h
	
	; set cram base addresses
	ld hl, VDP_WRITE_REGISTER | (3 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set bg tile addresses
	ld hl, VDP_WRITE_REGISTER | (4 << 8) | 0x7
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set sprite table addresses
	ld hl, VDP_WRITE_REGISTER | (5 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set sprite tile addresses
	ld hl, VDP_WRITE_REGISTER | (6 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; clear vram
	ld hl, VDP_WRITE_ADDRESS | 0x0000
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld c, VDP_DATA_PORT

	ld de, 0x4000
	clear_vram_loop:
		xor a, a
		out (c), a
		dec de
		ld a, d
		or a, e
		jr nz, clear_vram_loop

	; clear input variables
	ld a, 0xff
	ld (input_last), a
	ld (input_pressed), a

	call banjo_queue_init

	; check whether we're on a game gear in game gear mode
	; and whether there's an fm unit installed
	call banjo_check_hardware

	ld a, (banjo_has_chips)
	and a, BANJO_HAS_OPLL
	jr z, banjo_init_no_fm

		; initialise channels and FM
		ld a, CHAN_COUNT
		ld l, BANJO_HAS_SN | BANJO_HAS_OPLL
		call banjo_init

		; initialise sfx variables
		call banjo_sfx_init

		; use fm song and sfx tables
		ld hl, song_table_fm
		call banjo_set_song_table

		ld hl, sfx_table_fm
		call banjo_set_sfx_table

		jr banjo_init_done

	banjo_init_no_fm:

		; initialise channels
		ld a, CHAN_COUNT_SN
		ld l, BANJO_HAS_SN
		call banjo_init

		; initialise sfx variables
		call banjo_sfx_init

		; use sn song and sfx tables
		ld hl, song_table_sn
		call banjo_set_song_table

		ld hl, sfx_table_sn
		call banjo_set_sfx_table

	banjo_init_done:

	ld a, 0x80
	ld (volume_control), a

	; queue up song 0
	ld a, 0
	call banjo_queue_song

	ld a, 0
	ld (tic), a

	; enable vdp screen 1 and enable vblank interrupt
	ld hl, VDP_WRITE_REGISTER | (1 << 8) | 0b11100000

	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ei

	wait_vblank:
	
		halt
		
		ld a, (tic)
		inc a
		ld (tic), a

		; get controller 1 input
		in a, (0xdc)
		ld c, a

		; this will combine the new input and the last input
		; so that only a bit which is 0 this frame and was 1 last frame
		; will be 0 this frame (i.e. the button was just pressed)
		ld a, (input_last)
		xor a, c
		cpl
		or a, c 
		ld (input_pressed), a

		; update input_last
		ld a, c
		ld (input_last), a

		; set background colour
		ld c, VDP_CONTROL_PORT

		; set background colour for debugging
		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x1
		out (c), l
		out (c), h

		; left button stops or starts play
		ld a, (input_pressed)
		and a, 0x04
		jr nz, +

			ld a, (song_playing)
			or a, a
			call z, banjo_song_resume
			call nz, banjo_song_stop

		+:

		; right button plays sfx
		ld a, (input_pressed)
		and a, 0x08
		jr nz, +

			ld a, 0
			call banjo_queue_sfx

		+:

		; button 1 unmutes channel 0
		ld a, (input_pressed)
		and a, 0x10
		jr nz, +

			ld a, 0
			call banjo_unmute_song_channel

		+:

		; button 2 mutes channel 0
		ld a, (input_pressed)
		and a, 0x20
		ld a, 0
		jr nz, +
			ld a, 3
			ld (0xffff), a
			ld a, 0
			call banjo_mute_song_channel
		+:

		; volume control
		ld a, (input_pressed)
		and a, 0x2
		jr nz, +
			ld a, (volume_control)

			; already zero?
			or a, a
			jr nz, vc_not_zero

				ld a, 0x80
				jr vc_done

			vc_not_zero:

			sub a, 0x10

			vc_done:
			ld (volume_control), a
			call banjo_set_song_master_volume
		+:

		; run update every frame!
		call banjo_update

		; reset background colour
		ld c, VDP_CONTROL_PORT

		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x0
		out (c), l
		out (c), h
		
		jp wait_vblank

.incdir "../../music_driver/banjo"
.include "banjo.asm"

.incdir "../../music_driver/sfx"
.include "banjo_sfx.asm"

.incdir "../../music_driver/opll"
.include "banjo_opll.asm"

.incdir "../../music_driver/opll_drums"
.include "banjo_opll_drums.asm"

.incdir "../../music_driver/sn"
.include "banjo_sn.asm"

.incdir "../../music_driver/queue"
.include "banjo_queue.asm"

; song table
; SONG_DEF(SONG_LABEL)
song_table_fm:
	SONG_DEF(cmajor)

song_table_sn:
	SONG_DEF(cmajor_sn)

; sfx table
; SFX_DEF(SFX_LABEL, SFX_PRIORITY)
sfx_table_fm:
	SFX_DEF(sfx_test, 10)

sfx_table_sn:
	SFX_DEF(sfx_test_sn, 10)
	

.BANK 2
.SLOT 2
.SECTION "Songs 2"
	.include "cmajor.asm"
	.include "sfx_test.asm"
.ENDS

.BANK 3
.SLOT 2
.SECTION "Songs 3"
	.include "cmajor_sn.asm"
	.include "sfx_test_sn.asm"
.ENDS