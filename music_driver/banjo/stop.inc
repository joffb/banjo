
; banjo sound driver
; Joe Kennedy 2023

; stop the currently playing song and mute all channels
banjo_song_stop:

    ; do nothing if a song isn't playing
    ld a, (song_playing)
    or a, a
    ret z

        call banjo_mute_song
        call banjo_update_song

        xor a, a
        ld (song_playing), a

        ret