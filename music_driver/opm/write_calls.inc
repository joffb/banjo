
; banjo sound driver
; Joe Kennedy 2025

; h: register
; l: value to write
banjo_opm_write:

    push bc
    push de
    push hl

    push hl

    ; select register
    ld a, (banjo_opm_slot)
    ld e, h
    ld hl, OPM_REG_PORT
    call BANJO_WRTSLT

    pop hl

    ; write value
    ld a, (banjo_opm_slot)
    ld e, l
    ld hl, OPM_DATA_PORT
    call BANJO_WRTSLT

    pop hl
    pop de
    pop bc

    ret

