
; banjo sound driver
; Joe Kennedy 2024

; out = ""; for (i = 0; i < 16; i++) { row = []; for (j = 1; j < 17; j++) { row.push((i * j) >> 4)} out += ".db " + row.join(", ") + "\n" } console.log(out) 
music_volume_lookup_table: 

    .db 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    .db 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    .db 0, 0,0,0,0,0,0,1,1,1,1,1,1,1,1,2
    .db 0, 0,0,0,0,1,1,1,1,1,2,2,2,2,2,3
    .db 0, 0,0,1,1,1,1,2,2,2,2,3,3,3,3,4
    .db 0, 0,0,1,1,1,2,2,2,3,3,3,4,4,4,5
    .db 0, 0,1,1,1,2,2,3,3,3,4,4,4,5,5,6
    .db 0, 0,1,1,2,2,3,3,3,4,4,5,5,6,6,7
    .db 0, 1,1,2,2,3,3,4,4,5,5,6,6,7,7,8
    .db 0, 1,1,2,2,3,3,4,5,5,6,6,7,7,8,9
    .db 0, 1,1,2,3,3,4,5,5,6,6,7,8,8,9,10
    .db 0, 1,2,2,3,4,4,5,6,6,7,8,8,9,10,11
    .db 0, 1,2,3,3,4,5,6,6,7,8,9,9,10,11,12
    .db 0, 1,2,3,4,4,5,6,7,8,8,9,10,11,12,13
    .db 0, 1,2,3,4,5,6,7,7,8,9,10,11,12,13,14
    .db 0, 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15

music_calculate_volume: 

	ld a, channel.volume(ix)
	and a, #0xf

	; if channel volume is 0 we can finish now
	ret z

	; volume macro active?
	bit CHAN_FLAG_BIT_VOLUME_MACRO, channel.flags(ix)
	jr z, mcv_no_volume_macro

		; if macro volume is 0 we can finish now
		ld l, a
		ld a, channel.volume_macro_vol(ix)
		or a, a
		ret z

			; scale channel volume by macro volume
			; macro volume should already be in the upper nibble
			or a, l
			add a, #<music_volume_lookup_table
			ld l, a
			adc a, #>music_volume_lookup_table
			sub a, l
			ld h, a
			ld a, (hl)

	mcv_no_volume_macro: 
    
	; master volume active?
	ld h, music_state.master_volume(iy)
	bit 7, h
	jr nz, mcv_no_master_volume

		; scale channel volume by master volume

		; prepare master volume value
		ld l, a
		ld a, h
		add a, a
		and a, #0xf0
		or a, l
		
		; index into volume table
		add a, #<music_volume_lookup_table
		ld l, a
		adc a, #>music_volume_lookup_table
		sub a, l
		ld h, a
		ld a, (hl)

	mcv_no_master_volume: 

	ret 

