
.define VDP_CONTROL_PORT 0xbf
.define VDP_DATA_PORT 0xbe

.define VDP_WRITE_ADDRESS 0x4000
.define VDP_WRITE_CRAM 0xc000
.define VDP_WRITE_REGISTER 0x8000


.MEMORYMAP
	SLOTSIZE $4000
	DEFAULTSLOT 0
	SLOT 0 $0000			; ROM slot 0.
	SLOT 1 $4000			; ROM slot 1.
	SLOT 2 $8000			; ROM slot 2
	SLOT 3 $C000			; RAM
.ENDME

.ROMBANKMAP
	BANKSTOTAL 4
	BANKSIZE $4000
	BANKS 4
.ENDRO

.org 0x0000
jp init

.RAMSECTION "Main Vars" bank 0 slot 3

	tic: db

	fm_state: db

	debug_channel: db
	debug_chan_address: dw
	debug_vram_address: dw

.ENDS

.org 0x0038

	vblank_interrupt:

	push af
	exx
	
	ld c, VDP_CONTROL_PORT
	
	; acknowledge interrupt
	in a, (c)
	
	exx
	pop af
	
	ei
	reti

.org 0x0066
	retn
	

init:
	di
	im 1

	ld sp, 0xdfff

	; set vdp mode 4
	ld hl, VDP_WRITE_REGISTER | (0 << 8) | 0b00000100
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; disable vdp screens for now
	ld hl, VDP_WRITE_REGISTER | (1 << 8) | 0b10000000
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h
	
	; set vdp nametable base addresses
	ld hl, VDP_WRITE_REGISTER | (2 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h
	
	; set cram base addresses
	ld hl, VDP_WRITE_REGISTER | (3 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set bg tile addresses
	ld hl, VDP_WRITE_REGISTER | (4 << 8) | 0x7
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set sprite table addresses
	ld hl, VDP_WRITE_REGISTER | (5 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; set sprite tile addresses
	ld hl, VDP_WRITE_REGISTER | (6 << 8) | 0xff
	
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	call debug_init
	
	ld a, CHANNEL_COUNT
	call banjo_init

	ld a, (fm_unit_present)
	or a, a
	jr z, queue_song_sn

		ld a, 0
		ld (queue_song), a
		jr queue_song_done

	queue_song_sn:

		ld a, 1
		ld (queue_song), a
	
	queue_song_done:
	ld a, 0
	ld (tic), a

	; enable vdp screen 1 and enable vblank interrupt
	ld hl, VDP_WRITE_REGISTER | (1 << 8) | 0b11100000

	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ei

	wait_vblank:
	
		halt
		
		ld a, (tic)
		inc a
		ld (tic), a

		cp a, 0xff
		jr nz, wait_vblank_no_sfx

			ld a, (fm_unit_present)
			or a, a
			jr z, vblank_sfx_sn

				ld a, 0
				ld (queue_sfx), a
				jr wait_vblank_no_sfx

			vblank_sfx_sn:

				ld a, 1
				ld (queue_sfx), a

		wait_vblank_no_sfx:
		
		; set background colour
		ld c, VDP_CONTROL_PORT

		; set background colour for debugging
		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x1
		out (c), l
		out (c), h

		call banjo_update

		; reset background colour
		ld c, VDP_CONTROL_PORT

		ld hl, VDP_WRITE_REGISTER | (0x7 << 8) | 0x0
		out (c), l
		out (c), h
		
		; draw debug screen
		ld a, (song_playing)
		or a, a
		call nz, draw_debug
		
		jr wait_vblank

debug_init:

	; write debug gfx data to ram
	ld hl, VDP_WRITE_ADDRESS | 0x0000
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld c, VDP_DATA_PORT
	ld hl, debug_tiles_gfx

	ld a, ((debug_tiles_pal - debug_tiles_gfx) / 256) + 1

	copy_debug_tiles_loop:
		ld b, 0
		otir
		dec a
		jr nz, copy_debug_tiles_loop

	; write debug palette data to cram
	ld hl, VDP_WRITE_CRAM
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld b, 16
	ld c, VDP_DATA_PORT
	ld hl, debug_tiles_pal
	otir

	; clear tilemap and sprites
	ld hl, VDP_WRITE_ADDRESS | 0x3800
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld c, VDP_DATA_PORT
	ld de, 2048

	clear_tilemap_loop:
		out (c), 0
		dec de
		ld a, d
		or a, e
		jr nz, clear_tilemap_loop

	; draw debug columns for song state
	ld hl, VDP_WRITE_ADDRESS | 0x3800
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld c, VDP_DATA_PORT

	ld b, 32
	ld hl, debug_state_columns
	debug_state_columns_loop:
		ld a, (hl)
		sub a, 65
		add a, 26
		out (c), a
		out (c), 0
		inc hl
		djnz debug_state_columns_loop	

	; draw debug columns for channels
	ld hl, VDP_WRITE_ADDRESS | 0x38c0
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	ld c, VDP_DATA_PORT

	ld b, 32
	ld hl, debug_columns
	debug_columns_loop:
		ld a, (hl)
		sub a, 65
		add a, 26
		out (c), a
		out (c), 0
		inc hl
		djnz debug_columns_loop	

	call reset_debug_variables

	ret

reset_debug_variables:

	; start by looking at channel 0
	ld a, 0
	ld (debug_channel), a
	
	; start by looking at first song channel
	ld hl, song_channels
	ld (debug_chan_address), hl

	; start drawing on 2nd row
	ld hl, VDP_WRITE_ADDRESS | 0x3900
	ld (debug_vram_address), hl

	ret

draw_debug:

	ld iy, song_state

	; get channel address into ix
	ld ix, (debug_chan_address)

	; get vram writing address into hl
	ld hl, (debug_vram_address)

	; we're now writing to that address in vram
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; draw debug channel
	ld c, VDP_DATA_PORT

	ld b, 3

	draw_debug_loop:
	
		ld a, (ix + channel.flags)
		and a, CHAN_FLAG_MUTED
		jr z, draw_debug_not_muted

			ld a, 3
			out (c), a
			out (c), 0 
			jr draw_debug_chan_type

		draw_debug_not_muted:

			ld a, (ix + channel.flags)
			and a, CHAN_FLAG_NOTE_ON
			srl a
			add a, 4
			out (c), a
			out (c), 0 

		draw_debug_chan_type:
		ld a, (ix + channel.type)
		add a, 6
		out (c), a
		out (c), 0

		ld a, (ix + channel.subchannel)
		add a, 16
		out (c), a
		out (c), 0

		ld a, (ix + channel.volume)
		and 0xf
		add a, 16
		out (c), a
		out (c), 0

		out (c), 0
		out (c), 0

		ld d, (ix + channel.instrument_num)
		call draw_debug_bcd_8bit 

		out (c), 0
		out (c), 0

		ld d, (ix + channel.midi_note)
		call draw_debug_bcd_8bit 

		out (c), 0
		out (c), 0

		ld e, (ix + channel.freq)
		ld d, (ix + channel.freq + 1)
		call draw_debug_bcd_16bit

		out (c), 0
		out (c), 0

		ld e, (ix + channel.target_freq)
		ld d, (ix + channel.target_freq + 1)
		call draw_debug_bcd_16bit

		out (c), 0
		out (c), 0

		ld a, (ix + channel.slide_type)
		add a, 9
		out (c), a
		out (c), 0

		ld d, (ix + channel.slide_amount)
		call draw_debug_bcd_8bit 

		out (c), 0
		out (c), 0

		ld a, (ix + channel.arpeggio_pos)
		add a, 16
		out (c), a
		out (c), 0

		ld a, (ix + channel.arpeggio)
		add a, 16
		out (c), a
		out (c), 0

		ld a, (ix + channel.arpeggio + 1)
		add a, 16
		out (c), a
		out (c), 0

		out (c), 0
		out (c), 0

		ld d, (ix + channel.port)
		call draw_debug_bcd_8bit 
		
		out (c), 0
		out (c), 0

		; check whether we've hit the channel count
		ld a, (debug_channel)
		inc a
		ld (debug_channel), a
		ld d, (iy + music_state.channel_count)
		cp a, d
		jr nz, debug_channel_next

			call reset_debug_variables

			; move back to first channel
			ld ix, (debug_chan_address)

			; move back to first row
			ld hl, (debug_vram_address)
			ld c, VDP_CONTROL_PORT
			out (c), l
			out (c), h
		
			; draw debug channel
			ld c, VDP_DATA_PORT

			jr draw_debug_done

		debug_channel_next:
			
			; get debug_vram_address in hl
			ld hl, (debug_vram_address)

			; move vram address along
			ld de, 64
			add hl, de
			ld (debug_vram_address), hl

			; move channel address along
			ld ix, (debug_chan_address)
			ld de, _sizeof_channel
			add ix, de
			ld (debug_chan_address), ix

		draw_debug_done:

		dec b
		jp nz, draw_debug_loop

	; draw song state info
	; we're now writing to that address in vram
	ld hl, VDP_WRITE_ADDRESS | 0x3840
	ld c, VDP_CONTROL_PORT
	out (c), l
	out (c), h

	; draw debug channel
	ld c, VDP_DATA_PORT

	out (c), 0
	out (c), 0

	ld d, (iy + music_state.order)
	call draw_debug_bcd_8bit

	out (c), 0
	out (c), 0

	ld d, (iy + music_state.line)
	call draw_debug_bcd_8bit

	out (c), 0
	out (c), 0

	ld d, (iy + music_state.tic)
	call draw_debug_bcd_8bit

	out (c), 0
	out (c), 0

	ld d, (iy + music_state.subtic)
	call draw_debug_bcd_8bit


	ret

draw_debug_bcd_8bit:

	ld a, d
	rrca
	rrca
	rrca
	rrca
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ld a, d
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ret


draw_debug_bcd_16bit:

	ld a, d
	rrca
	rrca
	rrca
	rrca
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ld a, d
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ld a, e
	rrca
	rrca
	rrca
	rrca
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ld a, e
	and a, 0xf
	add a, 16
	out (c), a
	out (c), 0

	ret

.incdir "../../music_driver/"
.include "music_driver.asm"

debug_columns:
	.db "   V IN NO FNUM TARG SAM ARP    "
debug_state_columns:
	.db " OR LI TC ST                    "

debug_tiles_gfx:
	.incbin "debug_tiles_gfx.bin"

debug_tiles_pal:
	.incbin "debug_tiles_pal.bin"

sfx_table:
	SFX_DEF(sfx_test, 10)
	SFX_DEF(sfx_test_sn, 10)

song_table:
	SONG_DEF(cmajor)
	SONG_DEF(cmajor_sn)


.BANK 2
.SLOT 2
.SECTION "Song 1"
	.include "cmajor.asm"
	.include "sfx_test.asm"
.ENDS

.BANK 3
.SLOT 2
.SECTION "Song 2"
	.include "cmajor_sn.asm"
	.include "sfx_test_sn.asm"
.ENDS