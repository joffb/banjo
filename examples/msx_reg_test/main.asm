
.define INIT32 0x6f
.define SETWRT 0x53

.define IO_VDP_DATA 0x98
.define IO_VDP_CONTROL 0x99

.define AY_REG_WRITE		0xa0
.define AY_DATA_WRITE		0xa1
.define AY_DATA_READ		0xa2

.MEMORYMAP
	SLOTSIZE $4000
	DEFAULTSLOT 0
	SLOT 0 $4000			; ROM slot 1.
	SLOT 1 $8000			; ROM slot 2
	SLOT 2 $C000			; RAM
.ENDME

.ROMBANKMAP
	BANKSTOTAL 2
	BANKSIZE $4000
	BANKS 2
.ENDRO

.RAMSECTION "Main Vars" bank 0 slot 2

	output: .ds 12
	tic: db
	input_last: db
	input_pressed: db

.ENDS

.org 0x0000
.db "AB"
.dw init
.dw 0
.dw 0
.dw 0
.db 0,0,0,0,0,0

init:
	di
	im 1

	ld sp, 0xf100

    ; SCREEN1 - 32x24 text mode
    call INIT32

	ld hl, 0x1800
	call SETWRT

	ld hl, 0xc000

	ld b, 6

	write_read_loop:

		ld a, b

		; writing to register b
		out (AY_REG_WRITE), a

		; write b to the register
		out (AY_DATA_WRITE), a

		; writing to register b
		out (AY_REG_WRITE), a

		; read it back
		in a, (AY_DATA_READ)

		; store in memory
		ld (hl), a
		inc hl

		; write hex characters
		; high nibble
		ld c, a
		rra
		rra
		rra
		rra
		and a, 0xf
		cp a, 0xa
		jr c, +

			; additional add for hex >= a
			add a, 7

		+:
		add a, 0x30
		out (IO_VDP_DATA), a

		; low nibble
		ld a, c
		and a, 0xf
		cp a, 0xa
		jr c, +

			; additional add for hex >= a
			add a, 7

		+:
		add a, 0x30
		out (IO_VDP_DATA), a

		; space
		ld a, 0x20
		out (IO_VDP_DATA), a

		djnz write_read_loop

    ei

	wait_vblank:
	
		halt
		
		jr wait_vblank