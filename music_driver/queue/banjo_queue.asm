
; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_wladx.inc"

.RAMSECTION "BANJO_RAM_QUEUE" APPENDTO "BANJO_RAM" FREE

	; write bytes here to queue up music or sfx
	queue_sfx: db
	queue_song: db

	; used to modify the loop type
	queue_sfx_loop: db
	queue_song_loop: db

	; pointers to song and sfx tables
	song_table_ptr: dw
	sfx_table_ptr: dw

.ENDS

.SECTION "BANJO_QUEUE" FREE

	.include "queue.inc"

.ENDS