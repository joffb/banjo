
; banjo sound driver
; Joe Kennedy 2023

; stop the currently playing song and mute all channels
_banjo_song_stop: 

    ; do nothing if a song isn't playing
    ld a, (song_playing)
    or a, a
    ret z

        push ix
        push iy

        ld ix, #_song_channels
        ld iy, #_song_state

		; address for call to `ret` to
		ld hl, #music_song_mute_call_done
		push hl

		; get pointer to channel init call
		ld l, music_state.song_mute(iy)
		ld h, music_state.song_mute+1(iy)

		; run call
		jp (hl)

		; then return here
		music_song_mute_call_done: 

        pop iy
        pop ix

        ld a, #0
        ld (song_playing), a

        ret 
