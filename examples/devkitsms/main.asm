;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.2.0 #13081 (MINGW64)
;--------------------------------------------------------
	.module main
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl ___SMS__SDSC_signature
	.globl ___SMS__SDSC_descr
	.globl ___SMS__SDSC_name
	.globl ___SMS__SDSC_author
	.globl ___SMS__SEGA_signature
	.globl _main
	.globl _banjo_fm_unit_present
	.globl _banjo_queue_sfx
	.globl _banjo_queue_song
	.globl _banjo_update
	.globl _banjo_init
	.globl _SMS_VRAMmemsetW
	.globl _SMS_setBGPaletteColor
	.globl _SMS_waitForVBlank
	.globl _SMS_VDPturnOnFeature
	.globl _tic
	.globl _song_channel_ptrs
	.globl _song_channels
	.globl _SMS_SRAM
	.globl _SRAM_bank_to_be_mapped_on_slot2
	.globl _ROM_bank_to_be_mapped_on_slot0
	.globl _ROM_bank_to_be_mapped_on_slot1
	.globl _ROM_bank_to_be_mapped_on_slot2
	.globl _sfx_table
	.globl _song_table
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
_SMS_VDPControlPort	=	0x00bf
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_ROM_bank_to_be_mapped_on_slot2	=	0xffff
_ROM_bank_to_be_mapped_on_slot1	=	0xfffe
_ROM_bank_to_be_mapped_on_slot0	=	0xfffd
_SRAM_bank_to_be_mapped_on_slot2	=	0xfffc
_SMS_SRAM	=	0x8000
_song_channels::
	.ds 288
_song_channel_ptrs::
	.ds 18
_tic::
	.ds 1
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;main.c:12: void main(void)
;	---------------------------------
; Function main
; ---------------------------------
_main::
;main.c:15: SMS_VRAMmemsetW(0, 0x0000, 16384);
	ld	hl, #0x4000
	push	hl
	ld	de, #0x0000
	ld	h, l
	call	_SMS_VRAMmemsetW
;main.c:17: SMS_setBGPaletteColor(0, RGBHTML(0x0000FF));
	ld	l, #0x30
;	spillPairReg hl
;	spillPairReg hl
	xor	a, a
	call	_SMS_setBGPaletteColor
;main.c:20: SMS_displayOn();
	ld	hl, #0x0140
	call	_SMS_VDPturnOnFeature
;main.c:22: banjo_init(CHANNEL_COUNT);
	ld	a, #0x09
	call	_banjo_init
;main.c:24: if (banjo_fm_unit_present())
	call	_banjo_fm_unit_present
	or	a, a
	jr	Z, 00102$
;main.c:26: banjo_queue_song(0);
	xor	a, a
	call	_banjo_queue_song
	jr	00103$
00102$:
;main.c:30: banjo_queue_song(1);
	ld	a, #0x01
	call	_banjo_queue_song
00103$:
;main.c:33: tic = 0;
	ld	hl, #_tic
	ld	(hl), #0x00
00110$:
;main.c:37: SMS_waitForVBlank();
	call	_SMS_waitForVBlank
;main.c:39: if (tic == 64)
	ld	a, (_tic+0)
	sub	a, #0x40
	jr	NZ, 00108$
;main.c:41: if (banjo_fm_unit_present())
	call	_banjo_fm_unit_present
	or	a, a
	jr	Z, 00105$
;main.c:43: banjo_queue_sfx(0);
	xor	a, a
	call	_banjo_queue_sfx
	jr	00108$
00105$:
;main.c:47: banjo_queue_sfx(1);
	ld	a, #0x01
	call	_banjo_queue_sfx
00108$:
;main.c:51: banjo_update();
	call	_banjo_update
;main.c:53: tic++;
	ld	hl, #_tic
	inc	(hl)
;main.c:55: }
	jr	00110$
_song_table:
	.db #0x00	; 0
	.db #0x02	; 2
	.dw _cmajor
	.db #0x00	; 0
	.db #0x03	; 3
	.dw _cmajor_sn
_sfx_table:
	.db #0x00	; 0
	.db #0x02	; 2
	.dw _sfx_test
	.db #0x00	; 0
	.db #0x03	; 3
	.dw _sfx_test_sn
	.area _CODE
__str_0:
	.ascii "joe k  "
	.db 0x00
__str_1:
	.ascii "banjo test"
	.db 0x00
__str_2:
	.db 0x00
	.area _INITIALIZER
	.area _CABS (ABS)
	.org 0x3FF0
___SMS__SEGA_signature:
	.db #0x54	; 84	'T'
	.db #0x4d	; 77	'M'
	.db #0x52	; 82	'R'
	.db #0x20	; 32
	.db #0x53	; 83	'S'
	.db #0x45	; 69	'E'
	.db #0x47	; 71	'G'
	.db #0x41	; 65	'A'
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x99	; 153
	.db #0x99	; 153
	.db #0x00	; 0
	.db #0x4b	; 75	'K'
	.org 0x3FD8
___SMS__SDSC_author:
	.ascii "joe k  "
	.db 0x00
	.org 0x3FCD
___SMS__SDSC_name:
	.ascii "banjo test"
	.db 0x00
	.org 0x3FCC
___SMS__SDSC_descr:
	.db 0x00
	.org 0x3FE0
___SMS__SDSC_signature:
	.db #0x53	; 83	'S'
	.db #0x44	; 68	'D'
	.db #0x53	; 83	'S'
	.db #0x43	; 67	'C'
	.db #0x01	; 1
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0xd8	; 216
	.db #0x3f	; 63
	.db #0xcd	; 205
	.db #0x3f	; 63
	.db #0xcc	; 204
	.db #0x3f	; 63
