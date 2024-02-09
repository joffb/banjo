
// References to Songs which will be matched up at link time
extern song_data_t const cmajor;
extern song_data_t const cmajor_sn;

// Song table
// SONG_DEF(SONG_LABEL, SONG_BANK)
song_t const song_table[] = {
	SONG_DEF(cmajor, 2),
	SONG_DEF(cmajor_sn, 3),
};

// References to SFX which will be matched up at link time
extern song_data_t const sfx_test;
extern song_data_t const sfx_test_sn;

// SFX table
// SFX_DEF(SFX_LABEL, SFX_BANK, SFX_PRIORITY)
song_t const sfx_table[] = {
	SFX_DEF(sfx_test, 2, 0),
	SFX_DEF(sfx_test_sn, 3, 0),
};