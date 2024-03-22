
; banjo sound driver
; Joe Kennedy 2024

; include full opll code
.ifdef INCLUDE_OPLL

music_volume_change_fm: 

    ; restore original hl
	ex de, hl

    ; update local volume variable
    inc hl
    ld a, (hl)
    and a, #0xf
    ld channel.volume(ix), a

    ; preserve volume in c
    ld c, a

    ; don't update chip volume if
    ; channel is muted, volume macro is active or note-on is false
    ld a, channel.flags(ix)
    xor a, #CHAN_FLAG_NOTE_ON
    and a, #CHAN_FLAG_MUTED+CHAN_FLAG_NOTE_ON+CHAN_FLAG_VOLUME_MACRO
    jr nz, mvc_fm_dont_update_chip

        ; select volume/patch register
        ld a, channel.subchannel(ix)
        add a, #0x30
        out (#OPLL_REG_PORT), a

        ; get volume back into a
        ; combine with patch number and write to chip
        ld a, c
        or a, channel.fm_patch_shifted(ix)
        out (#OPLL_DATA_PORT), a

    mvc_fm_dont_update_chip: 
	inc hl
	jp music_process_new_line_subloop

music_volume_change_fm_drums: 

    ; restore original hl
	ex de, hl

    ; update local volume variable
    inc hl
    ld a, (hl)
    ld channel.volume(ix), a

	inc hl
	jp music_process_new_line_subloop

; don't include full opll code
.else 

music_volume_change_fm: 
music_volume_change_fm_drums: 
    inc hl
	jp music_process_new_line_subloop

.endif 
