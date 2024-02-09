
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


	ld a, CHANNEL_COUNT
	call banjo_init

	; check if an fm unit is installed
	; and play different songs depending
	ld a, (fm_unit_present)
	or a, a
	jr z, queue_song_sn

		ld a, 0
		ld (queue_song), a
		jr queue_song_done

	queue_song_sn:

		ld a, 1
		ld (queue_song), a
	
	queue_song_done:
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

		cp a, 0xff
		jr nz, wait_vblank_no_sfx

			ld a, (fm_unit_present)
			or a, a
			jr z, vblank_sfx_sn

				ld a, 0
				ld (queue_sfx), a
				jr wait_vblank_no_sfx

			vblank_sfx_sn:

				ld a, 1
				ld (queue_sfx), a

		wait_vblank_no_sfx:
		
		; set background colour
		ld c, VDP_CONTROL_PORT

		; set background colour for debugging
		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x1
		out (c), l
		out (c), h

		call banjo_update

		; reset background colour
		ld c, VDP_CONTROL_PORT

		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x0
		out (c), l
		out (c), h
		
		jr wait_vblank


; set channel count
.define CHANNEL_COUNT 9

.incdir "../../music_driver/"
.include "music_driver.asm"


; sfx table
; SFX_DEF(SFX_LABEL, SFX_PRIORITY)
sfx_table:
	SFX_DEF(sfx_test, 10)
	SFX_DEF(sfx_test_sn, 10)

; song table
; SONG_DEF(SONG_LABEL)
song_table:
	SONG_DEF(cmajor)
	SONG_DEF(cmajor_sn)

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