
.define VDP_CONTROL_PORT 0xbf
.define VDP_DATA_PORT 0xbe

.define VDP_WRITE_ADDRESS 0x4000
.define VDP_WRITE_CRAM 0xc000
.define VDP_WRITE_REGISTER 0x8000


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

.org 0x0000
jp init

.RAMSECTION "Main Vars" bank 0 slot 3

	tic: db
	input_last: db
	input_pressed: db

.ENDS

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

	; check whether we're on a game gear in game gear mode
	; and whether there's an fm unit installed
	call banjo_check_hardware

	ld a, 2
	call banjo_init

	; change bank
	ld a, :cmajor_sn
	ld (SLOT_2_BANK_CHANGE), a

	; get pointer in hl and loop mode on stack
	ld hl, cmajor_sn
	ld a, 1
	push af
	call banjo_play_song

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

		; button 1 stops or starts play
		ld a, (input_pressed)
		and a, 0x10
		jr nz, +

			ld a, (song_playing)
			or a, a
			call z, banjo_song_resume
			call nz, banjo_song_stop

		+;

		; button 2 plays an sfx
		ld a, (input_pressed)
		and a, 0x20
		jr nz, dont_play_sfx

			; change bank
			ld a, :sfx_test_sn
			ld (SLOT_2_BANK_CHANGE), a

			; get pointer in hl and loop mode on stack
			ld hl, sfx_test_sn
			ld a, 0
			push af
			call banjo_play_sfx

		dont_play_sfx:

		; change bank
		ld a, :cmajor_sn
		ld (SLOT_2_BANK_CHANGE), a
		call banjo_update_song

		; change bank
		ld a, :sfx_test_sn
		ld (SLOT_2_BANK_CHANGE), a
		call banjo_update_sfx

		; reset background colour
		ld c, VDP_CONTROL_PORT

		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x0
		out (c), l
		out (c), h
		
		jr wait_vblank

.incdir "../../music_driver/"
.include "music_driver.asm"

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