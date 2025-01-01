
; banjo sound driver
; Joe Kennedy 2024

; b: ix + channel.events
; ix: channel
; iy: music state
banjo_mute_channel_sn:

    ; silence channel
    ld a, (ix + channel.subchannel)
    or a, 0x9f

    ; get port
    ld c, (ix + channel.port)

    ; write to chip
    out (c), a
    
    ret

; ix: channel
; iy: music state
banjo_unmute_channel_sn:

    ; reset muted bit
    res CHAN_FLAG_BIT_MUTED, (ix + channel.flags)

    ret

; mute all SN channels
banjo_mute_all_sn:

	ld c, SN76489_PORT

	banjo_mute_all_sn_common:

		; mute all psg channels
		ld a, 0x9f | (0 << 5)
		out (c), a

		ld a, 0x9f | (1 << 5)
		out (c), a

		ld a, 0x9f | (2 << 5)
		out (c), a

		ld a, 0x9f | (3 << 5)
		out (c), a

		ret

; mute all SN channels
; writes to both the top SN port for the SMS
; and the port for the second SN in the System E
banjo_mute_all_sn_2:

	ld c, SN76489_2_PORT
	jr banjo_mute_all_sn_common

; mute all SN channels
; writes to both the top SN port for the SMS
; and the port for the second SN in the System E
banjo_mute_all_sn_3:

	ld c, SN76489_3_PORT
	jr banjo_mute_all_sn_common

; mute all SN channels
; writes to both the top SN port for the SMS
; and the port for the second SN in the System E
banjo_mute_all_sn_4:

	ld c, SN76489_4_PORT
	jr banjo_mute_all_sn_common