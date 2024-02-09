;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.2.0 #13081 (MINGW64)
;--------------------------------------------------------
	.module cmajor_sn
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _cmajor_sn_patterns_3_0
	.globl _cmajor_sn_patterns_2_0
	.globl _cmajor_sn_patterns_1_0
	.globl _cmajor_sn_patterns_0_1
	.globl _cmajor_sn_patterns_0_0
	.globl _cmajor_sn_orders_channel_3
	.globl _cmajor_sn_orders_channel_2
	.globl _cmajor_sn_orders_channel_1
	.globl _cmajor_sn_orders_channel_0
	.globl _cmajor_sn_order_pointers
	.globl _cmajor_sn_instrument_12
	.globl _cmajor_sn_instrument_11
	.globl _cmajor_sn_instrument_10
	.globl _cmajor_sn_instrument_9
	.globl _cmajor_sn_instrument_8
	.globl _cmajor_sn_instrument_7
	.globl _cmajor_sn_instrument_6
	.globl _cmajor_sn_instrument_5
	.globl _cmajor_sn_instrument_4
	.globl _cmajor_sn_instrument_3
	.globl _cmajor_sn_instrument_2
	.globl _cmajor_sn_instrument_1
	.globl _cmajor_sn_instrument_0
	.globl _cmajor_sn_instrument_pointers
	.globl _cmajor_sn_volume_macro_6
	.globl _cmajor_sn_volume_macro_5
	.globl _cmajor_sn_volume_macro_4
	.globl _cmajor_sn_volume_macro_3
	.globl _cmajor_sn_volume_macro_2
	.globl _cmajor_sn_volume_macro_1
	.globl _cmajor_sn_volume_macro_0
	.globl _cmajor_sn
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
_cmajor_sn:
	.db #0xba	; 186
	.db #0x00	; 0
	.db #0x04	; 4
	.db #0x01	; 1
	.db #0xff	; 255
	.db #0x01	; 1
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0x06	; 6
	.db #0x06	; 6
	.db #0x10	; 16
	.db #0x04	; 4
	.dw _cmajor_sn_instrument_pointers
	.dw _cmajor_sn_order_pointers
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x01	; 1
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0x00	; 0
	.db #0x03	; 3
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
_cmajor_sn_volume_macro_0:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x01	; 1
	.db #0x05	; 5
	.db #0x08	; 8
	.db #0x0d	; 13
	.db #0x0f	; 15
	.db #0x0f	; 15
_cmajor_sn_volume_macro_1:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x02	; 2
	.db #0x06	; 6
	.db #0x08	; 8
	.db #0x0a	; 10
	.db #0x0b	; 11
	.db #0x0d	; 13
	.db #0x0d	; 13
	.db #0x0e	; 14
	.db #0x0f	; 15
	.db #0x0f	; 15
	.db #0x0f	; 15
	.db #0x0f	; 15
_cmajor_sn_volume_macro_2:
	.db #0x02	; 2
	.db #0x07	; 7
	.db #0x0d	; 13
	.db #0x0f	; 15
_cmajor_sn_volume_macro_3:
	.db #0x0a	; 10
	.db #0x09	; 9
	.db #0x08	; 8
	.db #0x07	; 7
	.db #0x06	; 6
	.db #0x05	; 5
	.db #0x04	; 4
	.db #0x05	; 5
	.db #0x06	; 6
	.db #0x07	; 7
	.db #0x08	; 8
	.db #0x09	; 9
_cmajor_sn_volume_macro_4:
	.db #0x03	; 3
	.db #0x05	; 5
	.db #0x06	; 6
	.db #0x07	; 7
	.db #0x08	; 8
	.db #0x0b	; 11
	.db #0x0f	; 15
_cmajor_sn_volume_macro_5:
	.db #0x02	; 2
	.db #0x05	; 5
	.db #0x03	; 3
	.db #0x08	; 8
	.db #0x05	; 5
	.db #0x0b	; 11
	.db #0x07	; 7
	.db #0x0d	; 13
	.db #0x0a	; 10
	.db #0x0e	; 14
	.db #0x0f	; 15
_cmajor_sn_volume_macro_6:
	.db #0x01	; 1
	.db #0x05	; 5
	.db #0x09	; 9
	.db #0x0a	; 10
	.db #0x0c	; 12
	.db #0x0d	; 13
	.db #0x0e	; 14
	.db #0x0e	; 14
_cmajor_sn_instrument_pointers:
	.dw _cmajor_sn_instrument_0
	.dw _cmajor_sn_instrument_1
	.dw _cmajor_sn_instrument_2
	.dw _cmajor_sn_instrument_3
	.dw _cmajor_sn_instrument_4
	.dw _cmajor_sn_instrument_5
	.dw _cmajor_sn_instrument_6
	.dw _cmajor_sn_instrument_7
	.dw _cmajor_sn_instrument_8
	.dw _cmajor_sn_instrument_9
	.dw _cmajor_sn_instrument_10
	.dw _cmajor_sn_instrument_11
	.dw _cmajor_sn_instrument_12
_cmajor_sn_instrument_0:
	.db #0x09	; 9
	.db #0x00	; 0
	.db #0x04	; 4
	.dw _cmajor_sn_volume_macro_0
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_1:
	.db #0x0f	; 15
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_1
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_2:
	.db #0x04	; 4
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_2
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_3:
	.db #0x0c	; 12
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_3
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_4:
	.db #0x07	; 7
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_4
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_5:
	.db #0x0b	; 11
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_5
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_6:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0x10	; 16
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_7:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x21	; 33
	.db #0x18	; 24
	.db #0x00	; 0
	.db #0xf2	; 242
	.db #0xf4	; 244
	.db #0xff	; 255
	.db #0xeb	; 235
_cmajor_sn_instrument_8:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0xff	; 255
	.db #0x1e	; 30
	.db #0x0a	; 10
	.db #0xc8	; 200
	.db #0x08	; 8
	.db #0x1b	; 27
	.db #0x0a	; 10
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_9:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0xff	; 255
	.db #0x6f	; 111	'o'
	.db #0x00	; 0
	.db #0x50	; 80	'P'
	.db #0x05	; 5
	.db #0xc0	; 192
	.db #0x01	; 1
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_10:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0x30	; 48	'0'
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_11:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.dw #0xffff
	.db #0x80	; 128
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_instrument_12:
	.db #0x08	; 8
	.db #0x00	; 0
	.db #0xff	; 255
	.dw _cmajor_sn_volume_macro_6
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_order_pointers:
	.dw _cmajor_sn_orders_channel_0
	.dw _cmajor_sn_orders_channel_1
	.dw _cmajor_sn_orders_channel_2
	.dw _cmajor_sn_orders_channel_3
_cmajor_sn_orders_channel_0:
	.dw _cmajor_sn_patterns_0_0
	.dw _cmajor_sn_patterns_0_0
	.dw _cmajor_sn_patterns_0_1
	.dw _cmajor_sn_patterns_0_0
_cmajor_sn_orders_channel_1:
	.dw _cmajor_sn_patterns_1_0
	.dw _cmajor_sn_patterns_1_0
	.dw _cmajor_sn_patterns_1_0
	.dw _cmajor_sn_patterns_1_0
_cmajor_sn_orders_channel_2:
	.dw _cmajor_sn_patterns_2_0
	.dw _cmajor_sn_patterns_2_0
	.dw _cmajor_sn_patterns_2_0
	.dw _cmajor_sn_patterns_2_0
_cmajor_sn_orders_channel_3:
	.dw _cmajor_sn_patterns_3_0
	.dw _cmajor_sn_patterns_3_0
	.dw _cmajor_sn_patterns_3_0
	.dw _cmajor_sn_patterns_3_0
_cmajor_sn_patterns_0_0:
	.db #0x02	; 2
	.db #0x03	; 3
	.db #0x09	; 9
	.db #0x04	; 4
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x27	; 39
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2c	; 44
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x29	; 41
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2e	; 46
	.db #0x0a	; 10
	.db #0x02	; 2
	.db #0x01	; 1
	.db #0xff	; 255
_cmajor_sn_patterns_0_1:
	.db #0x02	; 2
	.db #0x0c	; 12
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x27	; 39
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2c	; 44
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2e	; 46
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x30	; 48	'0'
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x32	; 50	'2'
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2b	; 43
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x30	; 48	'0'
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0x90	; 144
	.db #0x2e	; 46
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_patterns_1_0:
	.db #0x02	; 2
	.db #0x04	; 4
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x0f	; 15
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x11	; 17
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x13	; 19
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x14	; 20
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x18	; 24
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x11	; 17
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x13	; 19
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x14	; 20
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x16	; 22
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xb0	; 176
	.db #0x1a	; 26
	.db #0xff	; 255
	.db #0xff	; 255
_cmajor_sn_patterns_2_0:
	.db #0x0a	; 10
	.db #0x0f	; 15
_cmajor_sn_patterns_3_0:
	.db #0x0a	; 10
	.db #0x0f	; 15
	.area _INITIALIZER
	.area _CABS (ABS)
