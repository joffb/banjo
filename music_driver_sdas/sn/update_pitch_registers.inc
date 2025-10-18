
; banjo sound driver
; Joe Kennedy 2024

; a: noise node
; iy: music_state
; ix: channel we're updating
music_update_noise_mode_sn_ch3_pitch: 

    ; using ch3 pitch

    ; clear "noise mode changed" flag
    res 7, music_state.noise_mode(iy)

    ; white noise or periodic noise?
    bit 0, a
    jr nz, munmsn_ch3_white

        ; ch3 pitch, periodic noise
        ld a, #0x80|(0x3<<5)|0x3
        out (#SN76489_PORT), a

        ret 

    munmsn_ch3_white: 
    
        ; ch3 pitch, white noise
        ld a, #0x80|(0x3<<5)|0x7
        out (#SN76489_PORT), a
    
        ret 
    
; a: noise node
; iy: music_state
; ix: channel we're updating
music_update_noise_mode_sn_fixed_pitch: 

    ; clear "noise mode changed" flag
    res 7, music_state.noise_mode(iy)

    ; preserve noise mode
    ld h, a

    ; get midi number
    ld a, channel.midi_note(ix)
                
    ; mod 12
    ld l, #12
    
    fixed_noise_mod_12: 
        sub a, l
        jr nc, fixed_noise_mod_12

    add a, l
    
    ; anything > 2 should be set to 2
    cp a, #3
    jr c, munmsn_fixed_lt_three

        ld a, #2

    munmsn_fixed_lt_three: 

    ; actual value should be 2 minus this value
    neg 
    add a, #2

    ; fixed pitch

    ; white noise or periodic noise?
    bit 0, h
    jr nz, munmsn_fixed_white

        ; fixed pitch, periodic noise
        or a, #0x80|(0x3<<5)
        out (#SN76489_PORT), a

        ret 

    munmsn_fixed_white: 
    
        ; fixed pitch, white noise
        or a, #0x80|(0x3<<5)|0x4
        out (#SN76489_PORT), a

        ret 

; b: current channel number
; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_sn: 

    ; check if this is the noise channel (need to shift the 3 by 5 as subchannel is pre-shifted)
    ld a, channel.subchannel(ix)
    cp a, #3<<5
    jr nz, mup_sn_square_channel

        ; we're in a noise channel
        ; get noise mode
        ld a, music_state.noise_mode(iy)

        ; fixed pitch mode or ch3 pitch mode?
        bit 1, a
        jr nz, mupr_sn_ch3_pitch

            call music_update_noise_mode_sn_fixed_pitch
            jp music_update_pitch_registers_sn_done

        mupr_sn_ch3_pitch: 

        ; for pitched noise it uses channel 3's pitch
        ; we want to write to the last square channel's pitch instead
        ld a, #0x2<<5

    ; for a regular square channel we update this channel's pitch
    mup_sn_square_channel: 

        ; get channel number (pre-shifted into correct position)
        ld c, a

    mup_sn_pitched_noise_channel: 

        ; get note fnum in de
        ld hl, #sn_tone_lookup
        call music_calc_fnum

        ; prepare note value for writing to chip
        ; isolate lower nibble
        ld a, e
        and a, #0xf
        
        ; set latch bit
        or a, #0x80
        ; combine with channel number
        or a, c

        ; output first byte of tone data        
        .ifdef BANJO_SMS

            out (#SN76489_PORT), a

        .else 

            ld c, channel.port(ix)
            out (c), a

        .endif 
        
        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call _banjo_sn_alf_wait
        .endif 

        ; get upper four bits of lower byte of fnum
        ; or with lower two bits of upper byte of fnum
        ld a, e
        and a, #0xf0
        or a, d

        ; rotate them into place
        rlca 
        rlca 
        rlca 
        rlca 

        ; output second byte of tone data
        .ifdef BANJO_SMS

            out (#SN76489_PORT), a

        .else 

            out (c), a

        .endif 

        ; ALF TEST
        ; additional write delay
        .ifdef BANJO_ALF
            call _banjo_sn_alf_wait
        .endif 

        ; done
        jp music_update_pitch_registers_sn_done
        
