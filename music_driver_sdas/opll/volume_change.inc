
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_opll: 

    ; choose volume register
    ld a, channel.subchannel(ix)
    add a, #0x30
    out (#OPLL_REG_PORT), a
    
    ; get channel volume
    ld a, channel.volume(ix)

.ifndef BANJO_MINIMAL

    ; check whether a volume macro is active and add the macro's value if it is
    bit CHAN_FLAG_BIT_VOLUME_MACRO, channel.flags(ix)
    jr z, mvcopll_no_macro

        and a, #0xf
        add a, channel.volume_macro_vol(ix)
        cp a, #0x10
        jr c, mvcopll_no_macro

            ; max out volume value at 0xf
            ld a, #0x0f

    mvcopll_no_macro: 

    ; is the master volume less than 0x80?
    bit 7, music_state.master_volume(iy)
    jr nz, mvcopll_no_master_volume

        ; preserve current volume
        ld c, a
        
        ; make master volume a 4 bit number
        ld a, music_state.master_volume(iy)
        rrca 
        rrca 
        rrca 
        and a, #0xf

        ; subtract from 15
        neg 
        add a, #0xf

        ; add to current volume total
        add a, c
        cp a, #0x10
        jr c, mvcopll_no_master_volume

            ; max out volume value at 0xf
            ld a, #0x0f

    mvcopll_no_master_volume: 

    ; preserve volume
    ld c, a

    ; ex macro active?
    bit CHAN_FLAG_BIT_EX_MACRO, channel.flags(ix)
    jr z, mvcopll_no_ex_macro

    ; is it a patch macro?
    ld a, channel.ex_macro_type(ix)
    cp a, #MACRO_TYPE_WAVE
    jr nz, mvcopll_no_ex_macro

        ; get current patch which is the upper nibble of the volume reg
        ; and combine with new volume level
        ld a, channel.ex_macro_val(ix)
        or a, c
        jr mvcopll_write

    mvcopll_no_ex_macro: 

.endif 

        ; get current patch which is the upper nibble of the volume reg
        ; and combine with new volume level
        or a, channel.patch(ix)

    mvcopll_write: 

        ; write new value
        out (#OPLL_DATA_PORT), a

        jp music_volume_change_opll_done
