
// References to Songs which will be matched up at link time
extern song_data_t const cmajor;
extern song_data_t const cmajor_sn;

// Song tables
// SONG_DEF(SONG_LABEL, SONG_BANK)

song_t const song_table_fm[] = {
	SONG_DEF(cmajor, 3),
};

song_t const song_table_sn[] = {
	SONG_DEF(cmajor_sn, 3),
};




// References to SFX which will be matched up at link time
extern song_data_t const sfx_test;
extern song_data_t const sfx_test_sn;

// SFX table
// SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY)
song_t const sfx_table_fm[] = {
	SFX_DEF(sfx_test, 2, 0),
};

song_t const sfx_table_sn[] = {
	SFX_DEF(sfx_test_sn, 3, 0),
};