
; banjo sound driver
; Joe Kennedy 2023

; check for presence of hardware and set bits in the banjo_has_chips
; variable, corresponding to what was found
banjo_check_hardware:

    ; clear the "has_chips" variable and system flags
    ld a, 0
    ld (banjo_has_chips), a
    ld (banjo_system_flags), a

    ; for SMS, assume the SN is present
    ld a, (banjo_has_chips)
    or a, BANJO_HAS_SN
    ld (banjo_has_chips), a

    ; get memory control register value which should have been set from the bios

    .ifdef BANJO_DEVKITSMS
        ; devkitsms preserves the value in a separate variable
        ld a, (_SMS_Port3EBIOSvalue)
    .endif

    .ifdef BANJO_GBDK
        ; gbdk preserves the value in a separate variable
        ld a, (__BIOS)
    .endif

    .ifndef BANJO_DEVKITSMS
        .ifndef BANJO_GBDK
            ld a, (0xc000)
        .endif
    .endif

    ; store the register value in our own variable
    ld (banjo_memory_control_value), a

    ; check for fm unit
    ; disable i/o
    set 2, a
    out (SMS_MEMORY_CONTROL_PORT), a

    ; get current state of control port and keep in d
    in a, (SMS_AUDIO_CONTROL_PORT)
    and a, 0x3
    ld d, a

    ; 4 loops, testing values [3, 2, 1, 0]
    ld b, 4

    ; count of correct read/write matches
    ld e, 0

    detect_fm_loop:

        ; output
        ld a, b
        dec a
        ld c, a
        out (SMS_AUDIO_CONTROL_PORT), a

        ; and read it back
        in a, (SMS_AUDIO_CONTROL_PORT)
        and a, 0x3

        ; matched?
        cp a, c
        jr nz, detect_fm_no_match

            ; increase count
            inc e

        detect_fm_no_match:
        djnz detect_fm_loop

    ; output original control port state
    ld a, d
    out (SMS_AUDIO_CONTROL_PORT), a

    ; reenable i/o
    ld a, (banjo_memory_control_value)
    out (SMS_MEMORY_CONTROL_PORT), a
    
    ; count = 2 implies mark 3 with fm (0, 1 will match)
    ; count = 4 implies sms with fm unit (0, 1, 2, 3 will match)
    ; counts of 1 or 3 imply no unit
    ld a, e
    cpl
    and a, 0x1
    jr z, bch_no_opll

        ; if bit 1 is set, the count is 2 which implies sega mark iii
        bit 1, e
        jr z, bch_not_mark3

            ; set flag to say we have a sega mark iii
            ld a, (banjo_system_flags)
            or a, SYS_FLAG_MARK_III
            ld (banjo_system_flags), a

        bch_not_mark3:

        ; set flag to say we have an OPLL fm unit
        ld a, (banjo_has_chips)
        or a, BANJO_HAS_OPLL
        ld (banjo_has_chips), a

    bch_no_opll:

    ; check for game gear mode

    ; read from i/o port 0
    ; if there's anything in the lowest 6 bits, we're not in game gear mode
    ; on sms 1 the port should read 0x78 which is the last byte of the  "in a, (c)"
    ; on sms 2 the port should read 0xff
    ; on game gear bits 0-5 should be empty and bits 6 and 7 depend on region and whether the start button is being held down

    ld c, GAME_GEAR_PORT_0
    in a, (c)

    ; isolate lower bits
    and a, 0x3f
    jr nz, bch_not_game_gear

        ; set flag to say we have a Game Gear
        ld a, (banjo_system_flags)
        or a, SYS_FLAG_GG
        ld (banjo_system_flags), a

    bch_not_game_gear:

    ret