
; banjo sound driver
; Joe Kennedy 2024

; a: number of channels
; l: chips to use
banjo_init_system_call:

    ; using chips to use parameter
    ; enable/disable the requested chips with the audio control port
    ; sn = 0x1, opll = 0x2
    ld a, l
    and a, BANJO_HAS_OPLL | BANJO_HAS_SN

    ; enable both opll and sn
    cp a, BANJO_HAS_OPLL | BANJO_HAS_SN
    jr z, bisc_sn_opll

    cp a, BANJO_HAS_SN
    jr z, bisc_sn_only

    cp a, BANJO_HAS_OPLL
    jr z, bisc_opll_only

    ; neither opll or sn selected, try to enable both
    bisc_sn_opll:
    ld a, 0x3
    jr bisc_write

    bisc_opll_only:
    ld a, 0x1
    jr bisc_write

    bisc_sn_only:
    ld a, 0x00

    bisc_write:
    out (SMS_AUDIO_CONTROL_PORT), a

    ret