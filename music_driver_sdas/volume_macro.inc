
; banjo sound driver
; Joe Kennedy 2024

music_update_volume_macro: 

    ; get current pointer to volume macro into hl
    ld l, channel.volume_macro_ptr(ix)
    ld h, channel.volume_macro_ptr+1(ix)
    
    ; check if we've reached the end of the macro and do nothing if we have
    ld a, channel.volume_macro_pos(ix)
    ld c, channel.volume_macro_len(ix)
    cp a, c
    jr nz, mpct_volume_macro_continue

        ; if loop == 0xff there's no loop so we're done
        ld a, channel.volume_macro_loop(ix)
        cp a, #0xff
        ret z

        ; keep loop value to subtract in c
        ld c, a

        ; store new volume macro position
        ld a, channel.volume_macro_pos(ix)
        sub a, c
        ld channel.volume_macro_pos(ix), a

        ; get lower byte of volume macro pointer in a
        ld a, l

        ; subtract loop point from current pointer
        ; to get new volume_macro_ptr
        sub a, c
        ld l, a
        ld a, h
        sbc a, #0
        ld h, a

    mpct_volume_macro_continue: 
    
    ; load new volume level into d
    ld d, (hl)
    
    ; check channel type
    ld a, channel.type(ix)
    
    ; TODO: apply volume macro to drums
    cp a, #CHAN_OPLL_DRUMS
    jr z, mpct_volume_macro_done
    
    ; set SN channel volume 
    cp a, #CHAN_SN76489
    jr nz, mpct_opll_channel
    
        ; add channel volume and macro volume together
        ld a, channel.volume(ix)
        and a, #0xf
        add a, d
        cp a, #0x10
        jr c, mpct_sn_macro_add_done

            ; max out volume value at 0xf
            ld a, #0x0f

        mpct_sn_macro_add_done: 

        ; get channel (pre-shifted into correct position)
        or a, channel.subchannel(ix)

        ; set latch and volume bit
        or a, #0x90
        
        ; load the port into c
        ld c, channel.port(ix)
        ; update chip
        out (c), a
    
        jr mpct_volume_macro_done
    
    ; set OPLL channel volume
    mpct_opll_channel: 
    cp a, #CHAN_OPLL
    jr nz, mpct_volume_macro_done
    
        ; choose volume register
        ld a, channel.subchannel(ix)
        add a, #0x30
        out (#OPLL_REG_PORT), a
        
        ; add channel volume and macro volume together
        ld a, channel.volume(ix)
        and a, #0xf
        add a, d
        cp a, #0x10
        jr c, mpct_fm_macro_add_done

            ; max out volume value at 0xf
            ld a, #0x0f

        mpct_fm_macro_add_done: 

        ; get current patch which is the upper nibble of the volume reg
        ; and combine with new volume level
        or a, channel.fm_patch_shifted(ix)
        
        ; write new value
        out (#OPLL_DATA_PORT), a
            
    mpct_volume_macro_done: 

        ; move volume macro position along and save it
        inc channel.volume_macro_pos(ix)
        
        ; move volume macro pointer along and save it
        inc hl
        ld channel.volume_macro_ptr(ix), l
        ld channel.volume_macro_ptr+1(ix), h

        ret 
