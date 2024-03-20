
// Song tables

// References to Songs which will be matched up at link time
extern song_data_t const cmajor;
extern song_data_t const cmajor_sn;

// FM Song table
// SONG_DEF(SONG_LABEL, SONG_BANK)
song_t const song_table_fm[] = {
	SONG_DEF(cmajor, 2),
};

// SN Song table
// SONG_DEF(SONG_LABEL, SONG_BANK)
song_t const song_table_sn[] = {
	SONG_DEF(cmajor_sn, 3),
};


// SFX tables

// References to SFX which will be matched up at link time
extern song_data_t const sfx_test;
extern song_data_t const sfx_test_sn;

// FM SFX table
// SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY)
song_t const sfx_table_fm[] = {
	SFX_DEF(sfx_test, 2, 0),
};

// SN SFX table
song_t const sfx_table_sn[] = {
	SFX_DEF(sfx_test_sn, 3, 0),
};