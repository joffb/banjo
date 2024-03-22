
; banjo sound driver
; Joe Kennedy 2023

; stop the currently playing song and mute all channels
_banjo_song_stop: 

    ; do nothing if a song isn't playing
    ld a, (song_playing)
    or a, a
    ret z

        ld iy, #_song_state

        ; does this song have fm channels? if so, mute them
        ld a, music_state.has_fm(iy)
        or a, music_state.has_fm_drums(iy)
        call nz, music_mute_all_fm

        ; does this song have sn channels? if so, mute them
        bit 0, music_state.has_sn(iy)
        call nz, music_mute_all_sn

        ld a, #0
        ld (song_playing), a

        ret 

; stop the currently playing sfx, muting its channel
; if a song is playing, unmute the corresponding channel
_banjo_sfx_stop: 

    ; do nothing if an sfx isn't playing
    ld a, (sfx_playing)
    or a, a
    jr z, banjo_sfx_stop_done

        ; mute the sfx channel
        ld hl, #_sfx_channel
        call music_mute_channel

        ; check if a song is playing
        ld a, (song_playing)
        or a, a
        jr z, banjo_sfx_stop_done
            
            ; get the sfx channel
            ld iy, #_sfx_state
            ld a, music_state.sfx_channel(iy)

            ; unmute the channel in the song
            call _banjo_unmute_song_channel

    banjo_sfx_stop_done: 

    ld a, #0
    ld (sfx_playing), a
    ld (sfx_priority), a
    
    ret 
