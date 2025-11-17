
; banjo sound driver
; Joe Kennedy 2025

; jumped to from music_update
; ix: channel
; iy: music state
banjo_update_channels_opna:

    ld a, OPN_REG_PORT
    ld (banjo_opn_port), a
    ld a, OPNA_REG_PORT
    ld (banjo_opna_port), a

    call music_update_channel_opn
    call music_update_channel_opn
    call music_update_channel_opn
    call music_update_channel_opn
    call music_update_channel_opn
    call music_update_channel_opn

    call music_update_channel_ay
    call music_update_channel_ay
    call music_update_channel_ay

    ; updates all rhythm channels
    call music_update_channel_opna_rhythm

    ret
