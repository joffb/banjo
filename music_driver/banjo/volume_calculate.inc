
; banjo sound driver
; Joe Kennedy 2024

; out = ""; for (i = 0; i < 16; i++) { row = []; for (j = 1; j < 17; j++) { row.push((i * j) >> 4)} out += ".db " + row.join(", ") + "\n" } console.log(out) 
music_volume_lookup_table:

    .db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    .db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    .db 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2
    .db 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3
    .db 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4
    .db 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5
    .db 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6
    .db 0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7
    .db 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8
    .db 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 9
    .db 0, 1, 1, 2, 3, 3, 4, 5, 5, 6, 6, 7, 8, 8, 9, 10
    .db 0, 1, 2, 2, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10, 11
    .db 0, 1, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12
    .db 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13
    .db 0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14
    .db 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15

music_calculate_volume:

	ld a, (ix + channel.volume)
	and a, 0xf
	
	ld de, music_volume_lookup_table

	bit CHAN_FLAG_BIT_VOLUME_MACRO, (ix + channel.flags)
	jr z, mcv_no_volume_macro

		or a, (ix + channel.volume_macro_vol)
		add a, e
		ld l, a
		adc a, d
		sub a, l
		ld h, a
		ld a, (hl)

	mcv_no_volume_macro:
    
	ld h, (iy + music_state.master_volume)
	bit 7, h
	jr nz, mcv_no_master_volume

		ld l, a
		ld a, h
		add a, a
		and a, 0xf0
		or a, l
		
		add a, e
		ld l, a
		adc a, d
		sub a, l
		ld h, a
		ld a, (hl)

	mcv_no_master_volume:

	ret
