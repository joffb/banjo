
; banjo sound driver
; Joe Kennedy 2024

; ix: channel
; iy: music state
; a - current volume (preserve this!)
music_update_volume_macro:

    ; get pointer to volume macro data in hl
    ld l, (ix + channel.volume_macro_ptr)
    ld h, (ix + channel.volume_macro_ptr + 1)

    muvm_get_value:

    ; get macro value into c
    ld c, (hl)

    ; move pointer forward
    inc hl

    ; if the value is negative (bit 7 set)
    ; it's a command rather than a value
    bit 7, c
    jr nz, muvm_command

        ; store the new pointer
        ld (ix + channel.volume_macro_ptr), l
        ld (ix + channel.volume_macro_ptr + 1), h

        ; subtract volume macro volume from current volume
        sub a, c
        jp music_update_volume_macro_done

    muvm_command:

        ; command 0 is an unconditional jump
        bit 0, c
        jr z, muvm_command_do_jump

        ; command 1 is a jump if we're not in envelope release
        bit CHAN_EVENT_BIT_MACRO_RELEASE, (ix + channel.events)
        jr z, muvm_command_do_jump

            ; we're in release
            ; skip past this jump
            inc hl
            inc hl

            jr muvm_get_value

        muvm_command_do_jump:

            ; get new address in hl
            ld c, (hl)
            inc hl
            ld h, (hl)
            ld l, c

            jr muvm_get_value
