
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opn:

	push bc

	; hl = pointer to op slot count for this algorithm, first slot reg, and tls for this patch
	; different algoritms can require a different number of tls (output volumes) to be updated
	ld l, (ix + channel.gp2)
	ld h, (ix + channel.gp3)

	; get op slot count
	ld b, (hl)
	inc hl

	; first slot reg
	ld d, (hl)
	inc hl

	; calculate volume w/ volume macros and master volume
    ld a, (iy + music_state.master_volume)
    ld c, 0x7f
    call music_calculate_volume_0x7f
    ld e, a

	; tl pointer into de
	; first slot reg into h
	ex de, hl

	opn_slot_volume_update_loop:

		push hl

		; get tl
		ld a, (de)
		and a, c

		; apply calculated volume to the tl value
		; new_tl = tl + (0x7f - vol)
		add a, c
		sub a, l

		; check if vol > 0x7f
        cp a, c
		jr c, muv_opn_not_neg

			; cap at 0x7f
			ld a, c

		muv_opn_not_neg:
		
		; register number in h
		; value to write in l
		ld l, a
		call banjo_opn_write_auto

		pop hl

		; add 4 to register numbe to move to next operator
		ld a, h
		add a, 0x4
		ld h, a

		djnz opn_slot_volume_update_loop

	pop bc

    jp music_volume_change_opn_done