
.include "../../music_driver/banjo_defines_wladx.inc"

.define INIT32 0x6f

.define CHAN_COUNT CHAN_COUNT_OPLL + CHAN_COUNT_AY

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

	tic: db
	input_last: db
	input_pressed: db

	song_channels: INSTANCEOF channel CHAN_COUNT
	song_channel_ptrs: ds CHAN_COUNT

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

	call banjo_check_hardware

	ld a, (banjo_has_chips)
	and a, BANJO_HAS_OPLL
	jr z, +

		ld a, CHAN_COUNT
		ld l, BANJO_HAS_OPLL
		call banjo_init

		; get pointer in hl and loop mode on stack
		ld hl, cmajor
		call banjo_play_song

		jr ++

	+:

		ld a, CHAN_COUNT_AY
		ld l, BANJO_HAS_AY
		call banjo_init

		; get pointer in hl and loop mode on stack
		ld hl, cmajor_ay
		call banjo_play_song

	++:

    ei

	wait_vblank:
	
		halt
		
        call banjo_update_song
		
		jr wait_vblank

.incdir "../../music_driver/banjo"
.include "banjo.asm"

.incdir "../../music_driver/ay"
.include "banjo_ay.asm"

.incdir "../../music_driver/opll"
.include "banjo_opll.asm"

.include "cmajor_ay_opll.asm"
.include "cmajor_ay.asm"
