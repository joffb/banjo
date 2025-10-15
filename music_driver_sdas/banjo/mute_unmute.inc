
; banjo sound driver
; Joe Kennedy 2023

; a: channel to mute
_banjo_mute_song_channel: 

    ; preserve channel number
    ld l, a

    ; check if the song is playing 
    ; do nothing if it isn't
    ld a, (song_playing)
    or a, a
    ret z

    push ix
    push iy

    ld iy, #_song_state

    ; jumped to from banjo_mute_song_channel_from_sfx in sfx module
    bmsc_sfx_jump: 

    ; restore channel number
    ld a, l

    ; a = channel_number * 2
	add a, a
    push af

    ; .assert  _sizeof_channel==32; ensure that the channel is the size we think it is!

    ; hl = channel_number * 32
    ld l, a
    ld h, #0
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl

    ; offset into song_channels
    ld de, #_song_channels
    add hl, de

    ; set muted flag and clear note-on flag
    set CHAN_FLAG_BIT_MUTED, (hl)
    res CHAN_FLAG_BIT_NOTE_ON, (hl)

    ; get channel into ix
    push hl
    pop ix

    ; get address of mute call table
    ld l, music_state.channel_mute_calls(iy)
    ld h, music_state.channel_mute_calls+1(iy)

    ; look up mute channel function in table
    ; a = channel_number * 2
    pop af
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; get address of function into hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; put return address on stack
    ld de, #_banjo_mute_song_channel_done
    push de

    ; call function
    jp (hl)

    _banjo_mute_song_channel_done: 

    pop iy
    pop ix

    ret 

; a: channel to mute
_banjo_unmute_song_channel: 

    ; .assert  _sizeof_channel==32; ensure that the channel is the size we think it is!

    ; hl = channel_number * 32
    ld l, a
    ld h, #0
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl

    ; offset into song_channels
    ld de, #_song_channels
    add hl, de

    ; clear muted flag
    res CHAN_FLAG_BIT_MUTED, (hl)
    
	ret 

_banjo_mute_song: 

    ; make sure a song is playing
    ld a, (song_playing)
    or a, a
    ret z

        push hl
        push iy

        ld iy, #_song_state

        ; set channel mute flags
        ld hl, #_song_channels
        ld de, #_sizeof_channel
        ld b, music_state.channel_count(iy)

        _banjo_mute_song_loop: 

            ; set muted flag and clear note-on flag
            set CHAN_FLAG_BIT_MUTED, (hl)
            res CHAN_FLAG_BIT_NOTE_ON, (hl)
            
            ; move to next channel
            add hl, de

            djnz _banjo_mute_song_loop

        ; get song mute call in hl
        ld l, music_state.song_mute(iy)
        ld h, music_state.song_mute+1(iy)

        ; set up return address
        ld de, #_banjo_mute_song_continue
        push de

        ; jump to song mute call
        jp (hl)

        _banjo_mute_song_continue: 

        pop iy
        pop hl

        ret 

_banjo_unmute_song: 

    ; make sure a song is playing
    ld a, (song_playing)
    or a, a
    ret z

        push hl
        push iy

        ld iy, #_song_state

        ; clear channel mute flags
        ld hl, #_song_channels
        ld de, #_sizeof_channel
        ld b, music_state.channel_count(iy)

        _banjo_unmute_song_loop: 

            ; unset channel muted flag
            res CHAN_FLAG_BIT_MUTED, (hl)
            
            ; move to next channel
            add hl, de

            djnz _banjo_unmute_song_loop

        pop iy
        pop hl

        ret 
