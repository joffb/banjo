
; banjo sound driver
; Joe Kennedy 2025

music_volume_change_opm:

	; hl = pointer to op slot count for this algorithm, first slot reg, and tls for this patch
	; different algoritms can require a different number of tls (output volumes) to be updated
	ld l, (ix + channel.gp2)
	ld h, (ix + channel.gp3)

	; get op slot count
	ld b, (hl)
	inc hl

	; first slot reg
	ld a, (hl)
    add a, (ix + channel.subchannel)
    ld d, a
	inc hl

	; tl pointer into de
	; first slot reg into h
	ex de, hl

	opm_slot_volume_update_loop:

        push bc
        push hl

		; get tl
		ld a, (de)
		and a, 0x7f

		; apply channel volume
		; out = vol - tl
		neg
		add a, (ix + channel.volume)

		; is this negative?
		jp p, muv_opm_not_neg

			; zero it if it is
			xor a,a 

		muv_opm_not_neg:
		
		; calculate volume w/ volume macro and master volume
		; preserve hl in de
		ld c, 0x7f
		call music_calculate_volume

		; invert back to a tl value
		neg
		add a, c
		
		; register number in h
		; value to write in l
		ld l, a
		call banjo_opm_write

        pop hl
        pop bc

		; add 4 to register numbe to move to next operator
		ld a, h
		add a, 0x8
		ld h, a

		djnz opm_slot_volume_update_loop

    jp music_volume_change_opm_done