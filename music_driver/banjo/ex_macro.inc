
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
; returns
; (hl) : new macro value
music_update_ex_macro:
    
    ; get macro position and pointer
    ; then run macro update
    ld c, (ix + channel.ex_macro_pos)
    ld e, (ix + channel.ex_macro_ptr)
    ld d, (ix + channel.ex_macro_ptr + 1)

    call music_macro_update

    ; save new macro position
    ld (ix + channel.ex_macro_pos), c

    ret