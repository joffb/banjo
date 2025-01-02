	
; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_sdas.inc"

.module BANJO_SFX

.area _DATA (REL,CON)

    ; state and channel data for sfx
	sfx_playing: .ds 1
	sfx_priority: .ds 1
    
	_sfx_state: .ds _sizeof_music_state
	_sfx_channel: .ds _sizeof_channel

.ifdef BANJO_GBDK
	.area _CODE_1 (REL,CON)
.else
	.area _CODE (REL,CON)
.endif
	
	.globl _sfx_state, _sfx_channel
	.globl sfx_playing, sfx_priority 
	.globl _banjo_sfx_init, _banjo_play_sfx, _banjo_update_sfx, _banjo_sfx_stop
	.globl _banjo_set_sfx_loop_mode, _banjo_set_sfx_master_volume

    .include "sfx.inc"
    