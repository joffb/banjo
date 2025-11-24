
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opl_2op:

    push bc

    ; get pointer to rest of patch data
    ; used for volume changes
    ld l, (ix + channel.gp2)
    ld h, (ix + channel.gp3)

    ; number of op/slots to update
    ld b, (hl)

    inc hl

    ; starting register
    ld a, (ix + channel.subchannel)
    call banjo_opl_2op_get_first_slot_index
    add a, (hl)
    ld d, a

    ; hl now pointing at tl values
    inc hl

	; calculate volume w/ volume macros and master volume
    ld a, (iy + music_state.master_volume)
    ld c, 0x3f
    call music_calculate_volume_0x3f
    ld e, a

    mvc_opl_2op_loop:

        ; preserve tl pointer in hl
        push hl

        ; set register
        ld a, d
        out (OPL_REG_PORT), a

		; get tl
		ld a, (hl)
		ld h, a

        ; keep around keyscale in l
        and a, 0xc0
        ld l, a

        ; tl in a
        ld a, h
        and a, c

		; apply calculated volume to the tl value
		; new_tl = tl + (0x3f - vol)
		add a, c
		sub a, e

		; check if vol > 0x3f
        cp a, c
		jr c, muv_opl_not_neg

			; cap at 0x3f
			ld a, c

		muv_opl_not_neg:
	
        ; combine with keyscale
        or a, l

        ; output new tl/ksr value
        out (OPL_DATA_PORT), a

        call banjo_opl_write_delay

        ; next register
        ld a, d
        add a, 3
        ld d, a

        ; restore tl pointer and move it on
        pop hl
        inc hl

        djnz mvc_opl_2op_loop

    pop bc

    jp music_volume_change_opl_2op_done
