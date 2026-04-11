
; banjo sound driver
; Joe Kennedy 2023

; check for presence of hardware and set bits in the banjo_has_chips
; variable, corresponding to what was found
banjo_check_hardware:
    
    ; for SMS, assume the SN is present
    ld a, BANJO_HAS_SN
    ld (banjo_has_chips), a
    
    ; clear the system flags
    xor a, a
    ld (banjo_system_flags), a

    .ifndef BANJO_NO_BIOS

        ; if we know we have a bios
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

    .endif

    .ifdef BANJO_NO_BIOS

        ; if we don't have a bios
        ; get memory control register value by checking all slots
        call banjo_get_memory_control_value

    .endif 

    ; store the register value in our own variable
    ld (banjo_memory_control_value), a

    ; check for fm unit
    ; disable i/o
    ld a, (banjo_memory_control_value)
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
    
    ; check the count of correct read/write matches
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

.ifdef BANJO_NO_BIOS

banjo_check_hardware_signature:

	.db 0xba, 0xba, 0xba, 0xba

banjo_find_current_slot:

	; slot select mask
	ld d, 0b11101111
	
	; port value with no slots enabled
	ld e, 0xeb

	banjo_find_current_slot_loop:

		; rotate mask
		rlc d
		jr nc, banjo_fcs_not_found

		; combine with port value with no slots enabled
		ld a, d
		and a, e
		out (0x3e), a

		ld a, 0xba
		ld b, 4
		ld hl, banjo_check_hardware_signature
		
		; check the signature matches what we expect
		banjo_fcs_signature_loop:
			cp a, (hl)
			inc hl
			jr nz, banjo_find_current_slot_loop	
			djnz banjo_fcs_signature_loop	
		
		; at this point we've detected the slot
		; get slot value into a and return
		ld a, d
		and a, e
		ret
		
		; if we can't find the slot, assume cartridge and hope for the best
		banjo_fcs_not_found:
		ld a, 0xab
		out (0x3e), a
		ret
		
banjo_find_current_slot_end:

banjo_get_memory_control_value:

	; ensure interrupts are disabled
	di 
	
	; copy check code to ram, relative to stack 
	ld hl, banjo_find_current_slot - banjo_find_current_slot_end - 2
	add hl, sp
	
	; preserve start address
	push hl
	
	; do the copy 
	ex de, hl
	ld hl, banjo_find_current_slot
	ld bc, banjo_find_current_slot_end - banjo_find_current_slot
	ldir
	
	; restore start address and jump to it
	; the code in ram will `ret` when it's done
	pop hl
	jp (hl)

.endif