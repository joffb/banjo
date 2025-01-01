	
; banjo sound driver
; Joe Kennedy 2023

.include "../banjo_defines_wladx.inc"

.RAMSECTION "BANJO_RAM_SFX" APPENDTO "BANJO_RAM" FREE

    ; state and channel data for sfx
	sfx_playing: db
	sfx_priority: db
	sfx_state INSTANCEOF music_state
	sfx_channel INSTANCEOF channel

.ENDS


.SECTION "BANJO_SFX" FREE

    .include "sfx.inc"

.ENDS
    