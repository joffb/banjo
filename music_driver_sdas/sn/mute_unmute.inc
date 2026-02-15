
; banjo sound driver
; Joe Kennedy 2024

; b: ix + channel.events
; ix: channel
; iy: music state
_banjo_mute_channel_sn: 

    ; silence channel
    ld a, channel.subchannel(ix)
    or a, #0x9f

    ; get port
    ld c, channel.port(ix)

    ; write to chip
    out (c), a
    
    ret 

; ix: channel
; iy: music state
_banjo_unmute_channel_sn: 

    ; reset muted bit
    res CHAN_FLAG_BIT_MUTED, channel.flags(ix)

    ret 

; mute all SN channels
_banjo_mute_all_sn: 

	ld c, #SN76489_PORT

	_banjo_mute_all_sn_common: 

		; mute all psg channels
		ld a, #0x9f|(0<<5)
		out (c), a

		ld a, #0x9f|(1<<5)
		out (c), a

		ld a, #0x9f|(2<<5)
		out (c), a

		ld a, #0x9f|(3<<5)
		out (c), a

		ret 


.ifdef BANJO_MULTIPLE_SN

	; mute all SN channels
	; writes to both the top SN port for the SMS
	; and the port for the second SN in the System E
	_banjo_mute_all_sn_2: 

		ld c, #SN76489_2_PORT
		jr _banjo_mute_all_sn_common

	; mute all SN channels
	; writes to both the top SN port for the SMS
	; and the port for the second SN in the System E
	_banjo_mute_all_sn_3: 

		ld c, #SN76489_3_PORT
		jr _banjo_mute_all_sn_common

	; mute all SN channels
	; writes to both the top SN port for the SMS
	; and the port for the second SN in the System E
	_banjo_mute_all_sn_4: 

		ld c, #SN76489_4_PORT
		jr _banjo_mute_all_sn_common

.endif 
