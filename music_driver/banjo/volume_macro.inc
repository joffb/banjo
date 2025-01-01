
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
; returns new volume level in a
music_update_volume_macro:
    
    ; check if we've reached the end of the macro
    ld a, (ix + channel.volume_macro_pos)
    cp a, (ix + channel.volume_macro_len)
    jr nz, mpct_volume_macro_continue

        ; if loop == 0xff there's no loop so we're done
        ld a, (ix + channel.volume_macro_loop)
        cp a, 0xff
        ret z

            ; reset position to loop point
            ld (ix + channel.volume_macro_pos), a

    mpct_volume_macro_continue:

        ; get current pointer to volume macro into hl
        add a, (ix + channel.volume_macro_ptr)
        ld l, a
        adc a, (ix + channel.volume_macro_ptr + 1) 
        sub a, l
        ld h, a

        ; store new macro volume
        ld a, (hl)
        ld (ix + channel.volume_macro_vol), a

        ; move volume macro position along and save it
        inc (ix + channel.volume_macro_pos)
    
        ; flag that the volume has changed
        set CHAN_EVENT_BIT_VOLUME_CHANGE, (ix + channel.events)

        ret