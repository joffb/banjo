; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_sdas.inc"

.module BANJO_QUEUE

.area _DATA (REL,CON)

	; write bytes here to queue up music or sfx
	queue_sfx: .ds 1
	queue_song: .ds 1

	; used to modify the loop type
	queue_sfx_loop: .ds 1
	queue_song_loop: .ds 1

	; pointers to song and sfx tables
	song_table_ptr: .ds 2
	sfx_table_ptr: .ds 2

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif

	.globl _banjo_queue_init, _banjo_update
	.globl _banjo_queue_song, _banjo_queue_sfx
	.globl _banjo_set_song_table, _banjo_set_sfx_table

	.include "queue.inc"