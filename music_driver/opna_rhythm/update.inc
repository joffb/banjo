
; banjo sound driver
; Joe Kennedy 2025


; ix: channel
; iy: music state
music_update_channel_opna_rhythm:

    ; clear note-on/off flags
    ld hl, 0
    ld (banjo_opna_rhythm_note_ons), hl

    ld b, 6

    muc_opna_r_loop:
        
        push bc

        ; process any events
        ld bc, music_command_jump_table_opna_rhythm
        call music_process_new_line

        ; move on to next channel
        ld de, _sizeof_channel
        add ix,de

        pop bc

        djnz muc_opna_r_loop

    ; update note-on/note-offs

	ld a, (banjo_opna_rhythm_note_ons)
	or a, a
	jr z, mu_opna_rhythm_note_ons_done
	
		; write note-ons (has a 576 cycle wait time apparently!)
		ld h, 0x10
		ld l, a
		call banjo_opn_write 
	
	mu_opna_rhythm_note_ons_done:

	ld a, (banjo_opna_rhythm_note_offs)
	or a, a
	jr z, mu_opna_rhythm_note_offs_done
	
		; set mute bit
		or a, 0x80
	
		; write note-offs (has a 576 cycle wait time apparently!)
		ld h, 0x10
		ld l, a
		call banjo_opn_write 
	
	mu_opna_rhythm_note_offs_done:

	ret