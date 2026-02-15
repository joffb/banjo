
; banjo sound driver
; Joe Kennedy 2024

; a: noise node
; iy: music_state
; ix: channel we're updating
music_update_noise_mode_sn_ch3_pitch:

    ; using ch3 pitch

    ; clear "noise mode changed" flag
    res 7, (iy + music_state.noise_mode)

    ; white noise or periodic noise?
    bit 0, a
    jr nz, munmsn_ch3_white

        ; ch3 pitch, periodic noise
        ld a, 0x80 | (0x3 << 5) | 0x3
        out (SN76489_PORT), a

        ret

    munmsn_ch3_white:
    
        ; ch3 pitch, white noise
        ld a, 0x80 | (0x3 << 5) | 0x7
        out (SN76489_PORT), a
    
        ret
    
; a: noise node
; iy: music_state
; ix: channel we're updating
music_update_noise_mode_sn_fixed_pitch:

    ; clear "noise mode changed" flag
    res 7, (iy + music_state.noise_mode)

    ; get flags
    ld l, (ix + channel.flags)

    ; preserve noise mode
    ld h, a

    ; check if an arpeggio effect is running
    bit CHAN_FLAG_BIT_ARPEGGIO, l
    jr z, munmsnfp_no_arpeggio

        call music_update_arpeggio
        add a, (ix + channel.midi_note)
        jr munmsnfp_arpeggio_done

    munmsnfp_no_arpeggio:

        ld a, (ix + channel.midi_note)

    munmsnfp_arpeggio_done:

    ; arp macro?
    bit CHAN_FLAG_BIT_ARP_MACRO, l
    jr z, munmsnfp_no_arp_macro

        ld l, a
        ld a, (ix + channel.arp_macro_val)

        ; if bit 7 is set, it's absolute
        bit 7, a
        jr z, munmsnfp_arp_macro_relative

            ; absolute arp
            and a, 0x7f
            
            jr munmsnfp_no_arp_macro

        munmsnfp_arp_macro_relative:

            ; relative arp macro
            ; sign extend bit 6 into bit 7
            rla
            sra a

            ; add note val
            add a, l

    munmsnfp_no_arp_macro:

    ; get note number mod 12
    sub a, 3
    add a, <music_divmod12
    ld e, a
    adc a, >music_divmod12
    sub a, e
    ld d, a
    ld a, (de)
    rrca
    rrca
    rrca
    rrca
    and a, 0xf

    ; anything > 2 should be set to 2
    cp a, 3
    jr c, munmsn_fixed_lt_three

        ld a, 2

    munmsn_fixed_lt_three:

    ; actual value should be 2 minus this value
    neg
    add a, 2

    ; fixed pitch

    ; white noise or periodic noise?
    bit 0, h
    jr nz, munmsn_fixed_white

        ; fixed pitch, periodic noise
        or a, 0x80 | (0x3 << 5)
        out (SN76489_PORT), a

        ret

    munmsn_fixed_white:
    
        ; fixed pitch, white noise
        or a, 0x80 | (0x3 << 5) | 0x4
        out (SN76489_PORT), a

        ret

; iy: music_state
; ix: channel we're updating
music_update_pitch_registers_sn:

    ; check if this is the noise channel (need to shift the 3 by 5 as subchannel is pre-shifted)
    ld a, (ix + channel.subchannel)
    cp a, 3 << 5
    jr nz, mup_sn_square_channel

        ; we're in a noise channel
        ; get noise mode
        ld a, (iy + music_state.noise_mode)

        ; fixed pitch mode or ch3 pitch mode?
        bit 1, a
        jr nz, mupr_sn_ch3_pitch

            call music_update_noise_mode_sn_fixed_pitch
            ret

        mupr_sn_ch3_pitch:

        ; for pitched noise it uses channel 3's pitch
        ; we want to write to the last square channel's pitch instead
        ld a, 0x2 << 5

    ; for a regular square channel we update this channel's pitch
    mup_sn_square_channel:

        ; get channel number (pre-shifted into correct position)
        ld c, a

    mup_sn_pitched_noise_channel:

        push bc

        ; calculate note number
        call music_calc_fnum

        ; turn that into an fnum and octave
        ld hl, sn_tone_lookup
        call music_fnum_lookup

        ; shift fnum to the correct octave
        call music_fnum_shift_octave

        pop bc

        ; prepare note value for writing to chip
        ; isolate lower nibble
        ld a, l
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
        ld a, l
        and a, 0xf0
        or a, h

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
        ret
        