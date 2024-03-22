
; banjo sound driver
; Joe Kennedy 2024

music_volume_change_sn: 

    ; restore original hl
	ex de, hl

	; update local volume variable
    inc hl
	ld a, (hl)
    ld channel.volume(ix), a

    ; preserve volume in c
    ld c, a

    ; don't update chip volume if
    ; channel is muted, volume macro is active or note-on is false
    ld a, channel.flags(ix)
    xor a, #CHAN_FLAG_NOTE_ON
    and a, #CHAN_FLAG_MUTED+CHAN_FLAG_NOTE_ON+CHAN_FLAG_VOLUME_MACRO
    jr nz, mvc_sn_dont_update_chip

        ; get volume back into a
        ; and write it to the chip
        ld a, c
        ld c, channel.port(ix)
        out (c), a

    mvc_sn_dont_update_chip: 
	inc hl
	jp music_process_new_line_subloop
