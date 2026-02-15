
; banjo sound driver
; Joe Kennedy 2025

mpnl_note_on_opna_rhythm:

    ; don't do anything if muted
    bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    jp nz, mpnl_command_done

        ; set note-on flag for this channel
        ld hl, banjo_opna_rhythm_note_ons
        ld a, (hl)
        or a, (ix + channel.gp1)
        ld (hl), a 

        jp mpnl_command_done


mpnl_note_off_opna_rhythm:

    ; don't do anything if muted
    bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    jp nz, mpnl_command_done

        ; set note-off flag for this channel
        ld hl, banjo_opna_rhythm_note_offs
        ld a, (hl)
        or a, (ix + channel.gp1)
        ld (hl), a 

        jp mpnl_command_done


mpnl_volume_change_opna_rhythm:

    ; prepare volume 
    ld a, (de)
    and a, 0x1f

    ; save in channel
    ld (ix + channel.volume), a

    ; don't do anything if muted
    bit CHAN_FLAG_BIT_MUTED, (ix + channel.flags)
    jp nz, mpnl_command_done
        
        ; panning
        or a, 0x80 | 0x40
        ld l, a
        
        ; get register number
        ld a, (ix + channel.subchannel)
        add a, 0x18
        ld h, a
        
        call banjo_opn_write
        
        jp mpnl_command_done