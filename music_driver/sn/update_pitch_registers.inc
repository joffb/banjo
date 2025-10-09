
; banjo sound driver
; Joe Kennedy 2024

; b: current channel number
; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_sn:

    ; check if this is the noise channel (need to shift the 3 by 5 as subchannel is pre-shifted)
    ld a, (ix + channel.subchannel)
    cp a, 3 << 5
    jr nz, mup_sn_square_channel

        ; we're in a noise channel
        ; check whether it's pitched or fixed
        ld a, (iy + music_state.noise_mode)
        ld d, a
        and a, 0x3
        cp a, 0x3
        jr nz, mup_sn_fixed_noise_channel

            ; we're in a pitched noise channel
            ; update noise mode
            .ifdef BANJO_SMS

                ld a, d
                out (SN76489_PORT), a

            .else

                ld c, (ix + channel.port)
                out (c), d

            .endif

            ; ALF TEST
            ; additional write delay
            .ifdef BANJO_ALF
                call banjo_sn_alf_wait
            .endif

            ; for pitched noise it uses channel 3's pitch
            ; we want to write to the last square channel's pitch instead
            ld c, 0x2 << 5
            jr mup_sn_pitched_noise_channel

    ; for a regular square channel we update this channel's pitch
    mup_sn_square_channel:

        ; get channel number (pre-shifted into correct position)
        ld c, a

    mup_sn_pitched_noise_channel:

        ; get note fnum in de
        ld hl, sn_tone_lookup
        call music_calc_fnum

        ; prepare note value for writing to chip
        ; isolate lower nibble
        ld a, e
        and a, 0xf
        
        ; set latch bit
        or a, 0x80
        ; combine with channel number
        or a, c

        ; output first byte of tone data        
        .ifdef BANJO_SMS

            out (SN76489_PORT), a

        .else

            ld c, (ix + channel.port)
            out (c), a

        .endif
        
        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call banjo_sn_alf_wait
        .endif

        ; get upper four bits of lower byte of fnum
        ; or with lower two bits of upper byte of fnum
        ld a, e
        and a, 0xf0
        or a, d

        ; rotate them into place
        rlca
        rlca
        rlca
        rlca

        ; output second byte of tone data
        .ifdef BANJO_SMS

            out (SN76489_PORT), a

        .else

            out (c), a

        .endif

        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call banjo_sn_alf_wait
        .endif

        ; done
        jp music_update_pitch_registers_sn_done
        
    ; pitch not based on channel 3
    mup_sn_fixed_noise_channel:
        
        ; get midi number
        ld a, (ix + channel.midi_note)
                    
        ; mod 12
        ld d, 12
        
        fixed_noise_mod_12:
            sub a, d
            jr nc, fixed_noise_mod_12

        add a, d
        
        ; restrict to 0-2
        and a, 0x3
        ld c, a

        ; actual value should be 2 minus this value
        ld a, 2
        sub a, c
        and a, 0x3
        ld c, a
    
        ; clear bottom 2 bits of current noise mode
        ; and or with new value
        ld a, (iy + music_state.noise_mode)
        or a, c
        
        ; update chip
        .ifdef BANJO_SMS

            out (SN76489_PORT), a

        .else

            ld c, (ix + channel.port)
            out (c), a

        .endif
    
        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call banjo_sn_alf_wait
        .endif

        ; done
        jp music_update_pitch_registers_sn_done
