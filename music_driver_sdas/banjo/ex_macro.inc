
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
music_update_ex_macro: 
    
    ; check if we've reached the end of the macro
    ld a, channel.ex_macro_pos(ix)
    or a, a
    ret z

        ; get pointer to ex macro data in hl
        ld l, channel.ex_macro_ptr(ix)
        ld h, channel.ex_macro_ptr+1(ix)
        
        ; move position on
        inc a

        ; hl is pointing at macro length
        ; compare our new pos with the length
        cp a, (hl)
        jr z, muem_at_end

            ; store the new pos
            ld channel.ex_macro_pos(ix), a
            
            muem_continue: 

            ; offset into macro data
            add a, l
            ld l, a
            adc a, h
            sub a, l
            ld h, a

            ; store new macro value
            ld a, (hl)
            ld channel.ex_macro_val(ix), a
        
            ; set event flag for this macro type
            ; (the event bit and type bit should be the same!)
            ld a, channel.ex_macro_type(ix)
            or a, channel.events(ix)
            ld channel.events(ix), a

            ret 

    muem_at_end: 

        ; pos has reached the end of the macro data
        ; move hl to macro loop and get the value
        inc hl
        ld a, (hl)

        ; store the new pos
        ld channel.ex_macro_pos(ix), a

        ; if the new pos == 0 then there's no looping
        or a, a
        ret z

        ;  move the ptr back
        dec a

        jr muem_continue

