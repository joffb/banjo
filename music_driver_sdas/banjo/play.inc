
; banjo sound driver
; Joe Kennedy 2023

; hl: pointer to music data
_banjo_play_song: 

	; try to ensure we're playing valid data
	ld a, (hl)
	cp a, #BANJO_MAGIC_BYTE
	ret nz

	; get channnel count
	; abort if this song uses too many channels
	inc hl
	inc hl
	ld a, (_banjo_max_channels)
	cp a, (hl)
	ret c

	; move pointer back
	dec hl
	dec hl

	; mark song as playing
	ld a, #1
	ld (song_playing), a

	; loop mode into b
	ld b, d

	push iy
	push ix

	ld ix, #_song_channels
	ld iy, #_song_state
	call music_play

	pop ix
	pop iy

	ret 


; b: loop mode
; ix: channels
; iy: state
; hl: pointer to music data
music_play: 

	; put music state header address into de
	push iy
	pop de

	; copy over song header
	ld bc, #_sizeof_music_state
	ldir 

	; init song position variables
	; check if we need to adjust the playback speeds from ntsc -> pal
	ld a, (music_framerate)
	cp a, #60
	jr z, music_play_same_speed
		
		; subtract 1 from speed 1 and speed 2 to roughly match playback
		; playing back on pal rather than ntsc
		ld a, music_state.speed_1(iy)
		dec a
		ld music_state.speed_1(iy), a
		
		ld a, music_state.speed_2(iy)
		dec a
		ld music_state.speed_2(iy), a

		
	music_play_same_speed: 
	
	; preserve channel data pointer
	push ix

	; point hl at the order_ptrs in the music_state
	push iy
	pop hl
	ld bc, #music_state.order_ptrs
	add hl, bc

	; point hl at the first channel's order pointer
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	; init channel data
	ld b, music_state.channel_count(iy)
		
	music_play_channel_data: 

		ld a, #0

		; clear all flags and events bits
		ld channel.flags(ix), a
		ld channel.events(ix), a
		
		; default line wait to 0
		ld channel.line_wait(ix), a

		; default tic wait to 0
		ld channel.tic_wait(ix), a

		; default arpeggios to 0
		ld channel.arpeggio(ix), a

		; default slide variables to 0
		ld channel.ex_macro_type(ix), a
		;
		; default volume macro length to 0
		ld channel.volume_macro_len(ix), a
		;
		; default vibrato_current and vibrato_depth to 0
		; ld (ix + channel.vibrato_depth), a

		; the following variables are initialised to 0xff
		ld a, #0xff

		; default instrument to no instrument (0xff)
		ld channel.instrument_num(ix), a

		; initialise freq to 0xffff
		ld channel.freq(ix), a
		ld channel.freq+1(ix), a

		; init pointer to order table for this channel
		; get pointer to order data in de
		ld e, (hl)
		ld channel.order_table_ptr(ix), e
		inc hl
		ld d, (hl)
		ld channel.order_table_ptr+1(ix), d
		inc hl
		
		; init address of first pattern in channel
		ld a, (de)
		ld channel.pattern_ptr(ix), a
		inc de
		ld a, (de)
		ld channel.pattern_ptr+1(ix), a
		
		; move to next channel
		ld de, #_sizeof_channel
		add ix, de

		djnz music_play_channel_data
	
	; restore channel data pointer
	pop ix

	;
	; init channel type data
	;

	; preserve channel data pointer
	push ix

	; address for call to `ret` to
	ld hl, #music_channel_init_call_done
	push hl

	; get pointer to channel init call
	ld l, music_state.channel_init_call(iy)
	ld h, music_state.channel_init_call+1(iy)

	; run call
	jp (hl)

	; then return here
	music_channel_init_call_done: 
		
	; restore channel data pointer
	pop ix

	; set process new line flag
	set STATE_FLAG_BIT_PROCESS_NEW_LINE, music_state.flags(iy)

	ret 

_banjo_song_rewind: 

	; check the song state to see if the first byte is the magic byte
	ld a, (_song_state+music_state.magic_byte)
	cp a, #BANJO_MAGIC_BYTE
	ret nz

		push ix
		push iy

		ld ix, #_song_channels
		ld iy, #_song_state
		call music_reset_channels_order_and_pattern

		ld a, #0
		ld music_state.tic(iy), a
		ld music_state.subtic(iy), a
		ld music_state.line(iy), a
		ld music_state.order(iy), a

		ld b, music_state.channel_count(iy)
		bsrw_channel_loop: 

			ld a, #0
			ld channel.flags(ix), #0
			ld channel.line_wait(ix), #0

			ld de, #_sizeof_channel
			add ix, de
			djnz bsrw_channel_loop

		pop iy
		pop ix

		ret 

; if there's a valid banjo song loaded, resume playback of it
_banjo_song_resume: 

	; check the song state to see if the first byte is the magic byte
	ld a, (_song_state+music_state.magic_byte)
	cp a, #BANJO_MAGIC_BYTE
	ret nz

		; resume playback
		ld a, #1
		ld (song_playing), a
	
		ret 

; a: loop mode
; 	   0 for loop off
; 	>= 1 for loop on
_banjo_set_song_loop_mode: 

	ld hl, #_song_state+music_state.flags

	; check loop mode
	or a, a
	jr nz, bsslm_loop_on

		; looping off
		res STATE_FLAG_BIT_LOOP, (hl)
		ret 

	bsslm_loop_on: 

		; looping on
		set STATE_FLAG_BIT_LOOP, (hl)
		ret 
