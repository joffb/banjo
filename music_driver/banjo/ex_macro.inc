
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
; returns new level in a
music_update_ex_macro:
    
    ; check if we've reached the end of the macro
    ld a, (ix + channel.ex_macro_pos)
    cp a, (ix + channel.ex_macro_len)
    jr nz, mpct_ex_macro_continue

        ; if loop == 0xff there's no loop so we're done
        ld a, (ix + channel.ex_macro_loop)
        cp a, 0xff
        ret z

            ; reset position to loop point
            ld (ix + channel.ex_macro_pos), a

    mpct_ex_macro_continue:

        ; get current pointer to ex macro into hl
        add a, (ix + channel.ex_macro_ptr)
        ld l, a
        adc a, (ix + channel.ex_macro_ptr + 1) 
        sub a, l
        ld h, a

        ; store new macro value
        ld a, (hl)
        ld (ix + channel.ex_macro_val), a

        ; move macro position along and save it
        inc (ix + channel.ex_macro_pos)

        ; set event flag for this macro type
        ; (the event bit and type bit should be the same!)
        ld a, (ix + channel.ex_macro_type)
        or a, (ix + channel.events)
        ld (ix + channel.events), a
        
        ret
