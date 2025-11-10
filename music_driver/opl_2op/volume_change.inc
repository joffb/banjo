
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opl_2op:

    ; get pointer to rest of patch data
    ; used for volume changes
    ld l, (ix + channel.gp2)
    ld h, (ix + channel.gp3)

    ; first byte has the algorithm in bit 0
    bit 0, (hl)
    inc hl
    jr z, mvco2_only_second_op

        call mvc02_apply_volume_operator
        ld c, a

        ; set register
        ld a, (ix + channel.subchannel)
        call banjo_opl_2op_get_first_slot_index
        add a, 0x40
        out (OPL_REG_PORT), a

        ; output new tl/ksr value
        ld a, c
        out (OPL_DATA_PORT), a

    mvco2_only_second_op:

        inc hl

        call mvc02_apply_volume_operator
        ld c, a

        ; set register
        ld a, (ix + channel.subchannel)
        call banjo_opl_2op_get_first_slot_index
        add a, 0x40 + 3
        out (OPL_REG_PORT), a

        ; output new tl/ksr value
        ld a, c
        out (OPL_DATA_PORT), a

        jp music_volume_change_opl_2op_done


mvc02_apply_volume_operator:

    ; get tl + ksr, mask out ksr
    ld a, (hl)
    and a, 0x3f

    ; apply channel volume
    ; out = vol - tl
    neg
    add a, (ix + channel.volume)

    ; is this negative?
    jp p, mvco2_so_not_neg

        ; zero it if it is
        xor a,a 

    mvco2_so_not_neg:

    ; calculate volume w/ volume macro and master volume
    ; preserve hl in de
    ex de, hl
    ld c, 0x3f
    call music_calculate_volume
    ex de, hl

    ; invert back to a tl value
    neg
    add a, c

    ; keep tl value in c, mask out ksr and recombine
    ld c, a
    ld a, (hl)
    and a, 0xc0
    or a, c

    ret