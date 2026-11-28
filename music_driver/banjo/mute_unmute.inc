
; banjo sound driver
; Joe Kennedy 2023

; a: channel to mute
banjo_mute_song_channel:

    ; channel number in hl
    ld l, a
    ld h, 0

    ; check if the song is playing 
    ; do nothing if it isn't
    ld a, (song_playing)
    or a, a
    ret z

        bmsc_sfx_jump:

        .assert _sizeof_channel == 32   ; ensure that the channel is the size we think it is!

        ; hl = channel_number * 32
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl
        add hl, hl

        ; offset into song_channels
        ld a, l
        add a, <song_channels
        ld l, a
        ld a, h
        adc a, >song_channels
        ld h, a

        ; set muted flag and note-off flag
        ; clear note-on flag
        ld a, (hl)
        res CHAN_FLAG_BIT_NOTE_ON, a
        or a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_OFF
        ld (hl), a

        ret


; a: channel to mute
banjo_unmute_song_channel:

    .assert _sizeof_channel == 32   ; ensure that the channel is the size we think it is!

    ; hl = channel_number * 32
    ld l, a
    ld h, 0
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl

    ; offset into song_channels
    ld a, l
    add a, <song_channels
    ld l, a
    ld a, h
    adc a, >song_channels
    ld h, a

    ; clear muted flag
    res CHAN_FLAG_BIT_MUTED, (hl)

    push ix
    push iy

    ; get channel into ix and song state into iy
    ; re-run instrument call to e.g. restore FM patch
    push hl
    pop ix
    ld iy, song_state
    call music_instrument_change_run_instrument_call

    pop iy
    pop ix
    
	ret


banjo_mute_song:

    ; make sure a song is playing
    ld a, (song_playing)
    or a, a
    ret z

        ; set channel mute flags
        ld hl, song_channels + channel.flags
        ld de, _sizeof_channel
        ld a, (song_state + music_state.channel_count)
        ld b, a

        banjo_mute_song_loop:

            ; set muted flag and note-off flag
            ; clear note-on flag
            ld a, (hl)
            res CHAN_FLAG_BIT_NOTE_ON, a
            or a, CHAN_FLAG_MUTED | CHAN_FLAG_NOTE_OFF
            ld (hl), a

            ; move to next channel
            add hl, de

            djnz banjo_mute_song_loop

        ret


banjo_unmute_song:

    ; make sure a song is playing
    ld a, (song_playing)
    or a, a
    ret z

        push ix
        push iy

        ; clear channel mute flags
        ld ix, song_channels
        ld iy, song_state
        ld de, _sizeof_channel
        ld b, (iy + music_state.channel_count)

        banjo_unmute_song_loop:

            ; unset channel muted flag
            res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

            ; re-run instrument call to e.g. restore FM patch
            call music_instrument_change_run_instrument_call

            ; move to next channel
            add ix, de

            djnz banjo_unmute_song_loop

        pop iy
        pop ix

        ret