
; banjo sound driver
; Joe Kennedy 2024

; set new master volume for the song
; values >= 0x80 are full volume
; lower values are attenuated
; a: new master volume
banjo_set_song_master_volume:

    ; flag that the master volume has been changed
    ld hl, song_state + music_state.flags
    set STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (hl)

    ; turn the volume into an attenuation
    cp a, 0x80
    jr c, bssmv_not_full

        ; no attenuation
        xor a, a
        jr bssmv_set_value

    bssmv_not_full:

        ; att = 0x7f - vol
        neg
        add a, 0x7f

    bssmv_set_value:

    ; store the new volume
    ld hl, song_state + music_state.master_volume
    ld (hl), a

    ret


; a: fade amount to subtract from master volume per frame
banjo_song_fade_out:

    neg

; a: fade amount to add to master volume per frame
banjo_song_fade_in:

    ld hl, song_state + music_state.master_volume_fade
    ld (hl), a

    ret

; update master volume fade
banjo_update_fade:

    push bc

    ld a, (iy + music_state.master_volume_fade)
    bit 7, a
    jr nz, buf_fade_out

        ; fading in
        add a, (iy + music_state.master_volume)
        
        ; if is this >= 0x80 we're done
        ; otherwise just update the volume
        bit 7, a
        jr z, buf_done

            ; set fade value to 0 and master volume to 0x80
            ld (iy + music_state.master_volume_fade), 0
            ld a, 0x80

            jr buf_done

    buf_fade_out:

        ; invert fade amount as we'll be using sub
        neg
        ld c, a

        ; fading out
        ld a, (iy + music_state.master_volume)
        sub a, c

        ; if there was no carry, it's still >0
        jr nc, buf_done

            ; if there was carry, we're done
            ld (iy + music_state.master_volume_fade), 0
            ld a, 0x0

    buf_done:

        ld (iy + music_state.master_volume), a
        set STATE_FLAG_BIT_MASTER_VOLUME_CHANGE, (iy + music_state.flags)

        pop bc

        ret