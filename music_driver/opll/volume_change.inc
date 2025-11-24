
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opll:

    ; choose volume register
    ld a, (ix + channel.subchannel)
    add a, 0x30
    out (OPLL_REG_PORT), a
    
    ; calculate volume w/ volume macros and master volume
    ld a, (iy + music_state.master_volume)
    ld c, 0xf
    call music_calculate_volume_0x0f
    
    ; convert to opll volume
    neg
    add a, c

    ; preserve volume
    ld l, a

    ; ex macro active?
    bit CHAN_FLAG_BIT_EX_MACRO, (ix + channel.flags)
    jr z, mvcopll_no_ex_macro

        ; is it a patch macro?
        bit MACRO_TYPE_BIT_WAVE, (ix + channel.ex_macro_type)
        jr nz, mvcopll_no_ex_macro

            ; get current patch which is the upper nibble of the volume reg
            ; and combine with new volume level
            ld a, (ix + channel.ex_macro_val)
            or a, l
            jr mvcopll_write

    mvcopll_no_ex_macro:

        ; get current patch which is the upper nibble of the volume reg
        ; and combine with new volume level
        ld a, (ix + channel.gp1)
        or a, l

    mvcopll_write:

        ; write new value
        out (OPLL_DATA_PORT), a

        jp music_volume_change_opll_done