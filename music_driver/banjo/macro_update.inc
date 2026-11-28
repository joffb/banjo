
; banjo sound driver
; Joe Kennedy 2026

; c: macro pos
; de: macro ptr
; returns
; (hl): macro value
; c: new macro pos
music_macro_update:

    ; offset into macro by position
    ld l, c
	ld h, 0
	add hl, de

	; it's a command if the value is >= 0xfe 
	ld a, (hl)
	cp a, 0xfe
	jr c, muvm_done

        ; move macro pointer on to the new position value
        inc hl

        ; check command type
        ; 0xff : jump
        inc a
        jr z, muvm_command_do_jump

        ; 0xfe : jump if envelope release isn't set
        bit CHAN_FLAG_BIT_MACRO_RELEASE, (ix + channel.flags)
        jr z, muvm_command_do_jump

            ; move pointer on to the next position value
            ; for when release is on
            inc hl

        muvm_command_do_jump:

            ; get new pos into c and hl
            ld c, (hl)
            ld l, c
            ld h, 0

            ; point hl at new value
            add hl, de

        muvm_done:

            ; move macro position on
            inc c
            
            ret