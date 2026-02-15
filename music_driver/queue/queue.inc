
; banjo sound driver
; Joe Kennedy 2024

; initialise queue variables
banjo_queue_init:

    ; initialise queues
    ld a, 0xff
    ld (queue_sfx), a
    ld (queue_sfx_loop), a
    ld (queue_song), a
    ld (queue_song_loop), a

    ret

; hl : address of song table
banjo_set_song_table:
    ld (song_table_ptr), hl
    ret 

; hl : address of sfx table
banjo_set_sfx_table:
    ld (sfx_table_ptr), hl
    ret

; a: song number to queue
banjo_queue_song:

    ld (queue_song), a

    ; clear loop mode modifier variable
    ld a, 0xff
    ld (queue_song_loop), a

    ret

; a: sfx number to queue
banjo_queue_sfx:

    ld (queue_sfx), a

    ; clear loop mode modifier variable
    ld a, 0xff
    ld (queue_sfx_loop), a

    ret

; a: loop mode for queued sfx
banjo_queue_song_loop_mode:

    ld (queue_song_loop), a

    ret

; a: loop mode for queued sfx
banjo_queue_sfx_loop_mode:

    ld (queue_sfx_loop), a

    ret

music_handle_sfx_queue:
    
    ld a, (queue_sfx)
    
    ; get address of queued sfx in table
    ; (multiply a by 4 and add to hl)
    ld hl, (sfx_table_ptr)
    add a, a
    add a, a
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; check priority of current sfx vs new priority
    ; don't play if if the priority is lower
    ld a, (sfx_priority)
    ld b, a
    ld a, (hl)
    cp a, b
    jr c, mhsfq_done
        
        ; update priority variable
        ld a, b
        ld (sfx_priority), a
        
        ; get bank and change slot 2 bank to this bank
        inc hl
        ld a, (hl)
        ld (SLOT_2_BANK_CHANGE), a

        ; preserve bank
        push af
        
        ; get address of sfx into hl
        inc hl
        ld a, (hl)
        inc hl
        ld h, (hl)
        ld l, a

        ; check if it's a valid song from the header
        ld a, (hl)
        cp a, BANJO_MAGIC_BYTE
        jr z, mhsfq_valid

            ; not valid, clear bank from stack
            pop af
            jp mhsfq_done

        mhsfq_valid:

            ld a, (queue_sfx_loop)
            ld b, a
            ld ix, sfx_channel
            ld iy, sfx_state
            call music_play

            ; update bank on music_state
            pop af
            ld (iy + music_state.bank), a

            ; sfx is now playing
            ld a, 1
            ld (sfx_playing), a
            
    mhsfq_done:
    
    ld a, 0xff
    ld (queue_sfx), a

    ret

music_handle_song_queue:

    ; check whether a song has been queued
    ld a, (queue_song)

    ; add offset to song table
    ld hl, (song_table_ptr)
    add a, a
    add a, a
    add a, l
    ld l, a
    adc a, h
    sub a, l
    ld h, a

    ; get bank and change slot 2 bank to this bank
    inc hl
    ld a, (hl)
    ld (SLOT_2_BANK_CHANGE), a

    ; preserve bank
    push af
    
    ; get address of song into hl
    inc hl
    ld a, (hl)
    inc hl
    ld h, (hl)
    ld l, a

    ; check if it's a valid song from the header
    ld a, (hl)
    cp a, BANJO_MAGIC_BYTE
    jr z, mhsoq_valid

    msoq_not_valid:

        ; not valid, clear bank from stack
        pop af
        jp mhsfq_done

    mhsoq_valid:

        ; get channnel count
        ; abort if this song uses too many channels
        inc hl
        inc hl
        ld a, (banjo_max_channels)
        cp a, (hl)
        jr c, msoq_not_valid

            ; move pointer back
            dec hl
            dec hl

            ld a, (queue_song_loop)
            ld b, a
            ld ix, song_channels
            ld iy, song_state
            call music_play

            ; update bank on music_state
            pop af
            ld (iy + music_state.bank), a
            
            ; song is now playing
            ld a, 1
            ld (song_playing), a

    mhsoq_done:

    ; clear song queue
    ld a, 0xff
    ld (queue_song), a
    
    ret


banjo_update:

    push ix
    push iy

    ; check if a song is queued
    ld a, (queue_song)
    cp a, 0xff
    jr nz, banjo_update_new_song

        ; song isn't queued, check if a song is playing
        ld a, (song_playing)
        or a, a
        jr z, banjo_update_check_sfx

            ; if it is, run the updates
            ld ix, song_channels
            ld iy, song_state

            ; change bank
            ld a, (iy + music_state.bank)
            ld (SLOT_2_BANK_CHANGE), a

            call music_update

            jr banjo_update_check_sfx

    banjo_update_new_song:

        ; new song has been queued
        call music_handle_song_queue

    banjo_update_check_sfx:

    ; check if an sfx is queued
    ld a, (queue_sfx)
    cp a, 0xff
    jr nz, banjo_update_new_sfx

        ; sfx isn't queued, check if a sfx is playing
        ld a, (sfx_playing)
        or a, a
        jr z, banjo_update_done

            ; if it is, run the updates
            ld ix, sfx_channel
            ld iy, sfx_state

            ; change bank
            ld a, (iy + music_state.bank)
            ld (SLOT_2_BANK_CHANGE), a

            call music_update

            jr banjo_update_done

    banjo_update_new_sfx:

        ; new sfx has been queued
        call music_handle_sfx_queue

    banjo_update_done:

    pop iy
    pop ix

    ret