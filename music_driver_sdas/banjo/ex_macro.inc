
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
; returns new level in a
music_update_ex_macro: 
    
    ; check if we've reached the end of the macro
    ld a, channel.ex_macro_pos(ix)
    cp a, channel.ex_macro_len(ix)
    jr nz, mpct_ex_macro_continue

        ; if loop == 0xff there's no loop so we're done
        ld a, channel.ex_macro_loop(ix)
        cp a, #0xff
        ret z

            ; reset position to loop point
            ld channel.ex_macro_pos(ix), a

    mpct_ex_macro_continue: 

        ; get current pointer to ex macro into hl
        add a, channel.ex_macro_ptr(ix)
        ld l, a
        adc a, channel.ex_macro_ptr+1(ix)
        sub a, l
        ld h, a

        ; store new macro value
        ld a, (hl)
        ld channel.ex_macro_val(ix), a

        ; move macro position along and save it
        inc channel.ex_macro_pos(ix)

        ; set event flag for this macro type
        ; (the event bit and type bit should be the same!)
        ld a, channel.ex_macro_type(ix)
        or a, channel.events(ix)
        ld channel.events(ix), a
        
        ret 
