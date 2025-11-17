
banjo_init_system_call:

    ; default mix to all just squares
    ld a, 7
    out (AY_REG_WRITE), a
	ld a, 0xf8
    out (AY_DATA_WRITE), a

	ret