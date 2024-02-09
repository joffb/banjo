;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.2.0 #13081 (MINGW64)
;--------------------------------------------------------
	.module sfx_test_sn
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _sfx_test_sn_patterns_2_0
	.globl _sfx_test_sn_orders_channel_2
	.globl _sfx_test_sn_order_pointers
	.globl _sfx_test_sn_instrument_0
	.globl _sfx_test_sn_instrument_pointers
	.globl _sfx_test_sn_volume_macro_0
	.globl _sfx_test_sn
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
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
	.area _BANK3
	.area _BANK3
_sfx_test_sn:
	.db #0xba	; 186
	.db #0x00	; 0
	.db #0x01	; 1
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x01	; 1
	.db #0x05	; 5
	.db #0x05	; 5
	.db #0x10	; 16
	.db #0x01	; 1
	.dw _sfx_test_sn_instrument_pointers
	.dw _sfx_test_sn_order_pointers
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_sfx_test_sn_volume_macro_0:
	.db #0x00	; 0
	.db #0x05	; 5
	.db #0x07	; 7
	.db #0x09	; 9
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0x04	; 4
	.db #0x0a	; 10
	.db #0x0a	; 10
	.db #0x0a	; 10
_sfx_test_sn_instrument_pointers:
	.dw _sfx_test_sn_instrument_0
_sfx_test_sn_instrument_0:
	.db #0x0a	; 10
	.db #0x00	; 0
	.db #0x06	; 6
	.dw _sfx_test_sn_volume_macro_0
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_sfx_test_sn_order_pointers:
	.dw _sfx_test_sn_orders_channel_2
_sfx_test_sn_orders_channel_2:
	.dw _sfx_test_sn_patterns_2_0
_sfx_test_sn_patterns_2_0:
	.db #0x02	; 2
	.db #0x00	; 0
	.db #0x08	; 8
	.db #0x07	; 7
	.db #0x0c	; 12
	.db #0x00	; 0
	.db #0xd0	; 208
	.db #0x37	; 55	'7'
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x00	; 0
	.db #0xd0	; 208
	.db #0x30	; 48	'0'
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x00	; 0
	.db #0xd0	; 208
	.db #0x2c	; 44
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x00	; 0
	.db #0xd0	; 208
	.db #0x29	; 41
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0x0a	; 10
	.db #0x03	; 3
	.area _INITIALIZER
	.area _CABS (ABS)
