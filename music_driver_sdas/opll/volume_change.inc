
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opll: 

    ; choose volume register
    ld a, channel.subchannel(ix)
    add a, #0x30
    out (#OPLL_REG_PORT), a
    
    ; get channel volume
    call music_calculate_volume
    
    ; convert to opll volume
    neg 
    add a, #15

    ; preserve volume
    ld l, a

    ; ex macro active?
    bit CHAN_FLAG_BIT_EX_MACRO, channel.flags(ix)
    jr z, mvcopll_no_ex_macro

        ; is it a patch macro?
        bit MACRO_TYPE_BIT_WAVE, channel.ex_macro_type(ix)
        jr nz, mvcopll_no_ex_macro

            ; get current patch which is the upper nibble of the volume reg
            ; and combine with new volume level
            ld a, channel.ex_macro_val(ix)
            or a, l
            jr mvcopll_write

    mvcopll_no_ex_macro: 

        ; get current patch which is the upper nibble of the volume reg
        ; and combine with new volume level
        ld a, channel.gp1(ix)
        or a, l

    mvcopll_write: 

        ; write new value
        out (#OPLL_DATA_PORT), a

        jp music_volume_change_opll_done
