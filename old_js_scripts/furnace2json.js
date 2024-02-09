const fs = require('fs');
const fsPromises = fs.promises;
const zlib = require('zlib');

const MAGIC_STRING = "-Furnace module-";

let song = {
	format_version: 0,
	song_info_pointer: 0,
	song_name: "",
	song_author: "",
	
	time_base: 0,
	speed_1: 0,
	speed_2: 0,
	tics_per_second: 0,
	
	pattern_length: 0,
	orders_length: 0,
	
	sound_chips: [],
	channel_count: 0,
	
	instrument_count: 0,
	instrument_pointers: [],
	instruments: [],
	
	wavetable_count: 0,
	wavetable_pointers: [],
	
	sample_count: 0,
	sample_pointers: [],
	
	pattern_count: 0,
	pattern_pointers: [],
	patterns: [],
	
	orders: [],
	
	// number of effect columns each channel has
	effect_columns: [],
}

// number of channels for each chip
let chip_channels = [];
chip_channels[0x03] = 4;	// 0x03: SMS (SN76489) - 4 channels
chip_channels[0x43] = 13;	// 0x43: SMS (SN76489) + OPLL (YM2413) - 13 channels (compound!)
chip_channels[0x89] = 9;	// 0x89: OPLL (YM2413) - 9 channels
chip_channels[0xa7] = 11;	// 0xa7: OPLL drums (YM2413) - 11 channels

let macro_word_size_convert = [1, 1, 2, 4];
let macro_word_read_function = ["readUint8", "readInt8", "readInt16LE", "readInt32LE"];

// return buffer with the unzipped furnace file data or false
async function load_file(filename)
{
	// file doesn't exist
	if (!fs.existsSync(filename))
	{
		return false;
	}
	
	let file = await fsPromises.readFile(filename);

	let magic_string = file.toString('utf8', 0, 16)
	
	// file needs to be unzipped
	if (magic_string != MAGIC_STRING)
	{		
		file = zlib.inflateSync(file);
		
		magic_string = file.toString('utf8', 0, 16);
		
		// not a valid furnace file it seems
		if (magic_string != MAGIC_STRING)
		{
			return false
		}
	}
	
	return file;
}

async function main (input_file, output_file)
{
	let file = await load_file(input_file);
	
	if (!file)
	{
		console.log("Not a valid furnace file");
		return;
	}
	
	song.format_version = file.readUInt16LE(16);
	song.song_info_pointer = file.readUInt32LE(20);
	
	song.time_base = file.readUInt8(song.song_info_pointer + 8);
	song.speed_1 = file.readUInt8(song.song_info_pointer + 9);
	song.speed_2 = file.readUInt8(song.song_info_pointer + 10);
	song.tics_per_second = file.readFloatLE(song.song_info_pointer + 12);
	
	song.pattern_length = file.readUInt16LE(song.song_info_pointer + 16);
	song.orders_length = file.readUInt16LE(song.song_info_pointer + 18);
	
	song.instrument_count = file.readUInt16LE(song.song_info_pointer + 22);
	song.wavetable_count = file.readUInt16LE(song.song_info_pointer + 24);
	song.sample_count = file.readUInt16LE(song.song_info_pointer + 26);
	song.pattern_count = file.readUInt32LE(song.song_info_pointer + 28);
	
	// get array of chips which are used by this song
	song.sound_chips = file.subarray(song.song_info_pointer + 32, song.song_info_pointer + 64);
	song.sound_chips = [...song.sound_chips].filter((chip) => (chip > 0));
	
	// calculate number of channels for this set of chips
	song.channel_count = song.sound_chips.reduce((acc, chip) => (acc += chip_channels[chip]), 0);
	
	// get song name from null terminated string
	let song_name_start = song.song_info_pointer + 256;
	let song_name_end = song.song_info_pointer + 256;
	
	while (file[song_name_end] != 0)
	{
		song_name_end++;
	}
	
	// get song author from null terminated string
	let song_author_start = song_name_end + 1;
	let song_author_end = song_author_start;
	
	while (file[song_author_end] != 0)
	{
		song_author_end++;
	}
	
	song.song_name = file.toString("utf8", song_name_start, song_name_end);
	song.song_author = file.toString("utf8", song_author_start, song_author_end);
	
	// instrument pointers start 24 bytes after the song author
	// pointers are 32 bit uints
	let instrument_pointers_start = song_author_end + 1 + 24;
	
	for (let i = 0; i < song.instrument_count; i++)
	{
		song.instrument_pointers.push(file.readUInt32LE(instrument_pointers_start + (i * 4)));
	}
	
	// wavetable pointers
	// pointers are 32 bit uints
	let wavetable_pointers_start = instrument_pointers_start + (4 * song.instrument_count);
	
	for (let i = 0; i < song.wavetable_count; i++)
	{
		song.wavetable_pointers.push(file.readUInt32LE(wavetable_pointers_start + (i * 4)));
	}
	
	// sample pointers
	// pointers are 32 bit uints
	let sample_pointers_start = wavetable_pointers_start + (4 * song.wavetable_count);
	
	for (let i = 0; i < song.sample_count; i++)
	{
		song.sample_pointers.push(file.readUInt32LE(sample_pointers_start + (i * 4)));
	}
	
	// pattern pointers
	// pointers are 32 bit uints
	let pattern_pointers_start = sample_pointers_start + (4 * song.sample_count);
	
	for (let i = 0; i < song.pattern_count; i++)
	{
		song.pattern_pointers.push(file.readUInt32LE(pattern_pointers_start + (i * 4)));
	}
	
	// orders
	// channel_count * orders_length array of pattern numbers
	let orders_start = pattern_pointers_start + (4 * song.pattern_count);
	
	for (let i = 0; i < song.channel_count; i++)
	{
		song.orders[i] = [];
		
		for (let j = 0; j < song.orders_length; j++)
		{
			song.orders[i].push(file.readUInt8(orders_start + (i * song.orders_length) + j));
		}		
	}
	
	// number of effect columns each channel has
	let effect_columns_start = orders_start + (song.channel_count * song.orders_length);
	
	for (let i = 0; i < song.channel_count; i++)
	{
		song.effect_columns[i] = file.readUInt8(effect_columns_start + i);
	}
	
	// init pattern arrays
	for (let i = 0; i < song.channel_count; i++)
	{
		song.patterns[i] = [];
	}

	console.log(song);
	
	// load patterns 
	if (song.format_version < 157)
	{
		for (let i = 0; i < song.pattern_count; i++)
		{
			let pattern_pointer = song.pattern_pointers[i];
			let pattern = {
				channel: file.readUInt16LE(pattern_pointer + 8),
				index: file.readUInt16LE(pattern_pointer + 10),
				data: [],
			};
			
			// size of each row in the pattern
			let pattern_row_size = (4 + (song.effect_columns[pattern.channel] * 2)) * 2;
			
			// get data for each row
			let pattern_data_pointer = pattern_pointer + 16;
			
			for (let j = 0; j < song.pattern_length; j++)
			{
				let pattern_row_pointer = pattern_data_pointer + (pattern_row_size * j);
				
				let data = {
					note: file.readInt16LE(pattern_row_pointer),
					octave: file.readInt16LE(pattern_row_pointer + 2),
					instrument: file.readInt16LE(pattern_row_pointer + 4),
					volume: file.readInt16LE(pattern_row_pointer + 6),
					effects: []
				};		
				
				// get effects for row
				for (let k = 0; k < song.effect_columns[pattern.channel]; k++)
				{
					data.effects.push(file.readInt8(pattern_row_pointer + 8 + (k * 2)));
					data.effects.push(file.readInt8(pattern_row_pointer + 1 + 8 + (k * 2)));
					
				}
				
				pattern.data.push(data);
			}
			
			// add to patterns list
			song.patterns[pattern.channel][pattern.index] = pattern;
		}
	}
	else if (song.format_version >= 157) 
	{
		for (let i = 0; i < song.pattern_count; i++)
		{
			let pattern_pointer = song.pattern_pointers[i];
			let pattern = {
				channel: file.readUInt8(pattern_pointer + 9),
				index: file.readUInt16LE(pattern_pointer + 10),
				name: "",
				data: [],
			};
			
			// get data for each row
			let pattern_data_pointer = pattern_pointer + 12;

			// pattern name
			let read_byte = "";

			while ((read_byte = file.readUInt8(pattern_data_pointer)) != 0)
			{
				pattern_name += read_byte;
				pattern_data_pointer++;
			}

			// pattern_data_pointer now pointing at pattern data
			pattern_data_pointer++;			
			
			let data_start_pointer = pattern_data_pointer;

			// keep reading bytes until we hit 0xff
			while ((read_byte = file.readUInt8(pattern_data_pointer)) != 0xff)
			{
				let data = {
					note: 0,
					octave: 0,
					instrument: -1,
					volume: -1,
					effects: []
				};

				// move to next byte
				pattern_data_pointer++;

				if (read_byte & 0x80)
				{
					// if bit 7 is set, then read bit 0-6 as "skip N+2 rows".
					for (let j = 0; j < ((read_byte & 0x7f) + 2);  j++)
					{
						pattern.data.push(data);
					}
				}
				else
				{
					// read the extra effect bytes if they're present
					//
					// - if bit 5 is set, read another byte:
					// extra effects 0-3
					// - if bit 6 is set, read another byte:
					// extra effects 4-7
					let effect_bytes = [];
					let extra_effect_byte_count = 0;

					extra_effect_byte_count += (read_byte & 0x20) ? 1 : 0;
					extra_effect_byte_count += (read_byte & 0x40) ? 1 : 0;

					// get effect specification bytes
					for (let j = 0; j < extra_effect_byte_count; j++)
					{
						effect_bytes.push(file.readUInt8(pattern_data_pointer));
						pattern_data_pointer++;
					}

					// bit 0: note present
					if (read_byte & 0x1)
					{
						data.note = file.readUInt8(pattern_data_pointer);
						pattern_data_pointer++;

						// convert note data to version compatible with older (< 157) pattern
						if (data.note > 0 && data.note <= 179)
						{
							data.octave = Math.floor(data.note / 12) - 5;
							data.note = data.note % 12;
						}
						else if (data.note >= 180)
						{
							data.note = data.note - 80;
						}
					}

					// bit 1: ins present
					if (read_byte & 0x2)
					{
						data.instrument = file.readUInt8(pattern_data_pointer);
						pattern_data_pointer++;
					}
					
					// bit 2: volume present
					if (read_byte & 0x4)
					{
						data.volume = file.readUInt8(pattern_data_pointer);
						pattern_data_pointer++;
					}

					// if bit 5 is present, we'll get the bytes for effect 0 at the next stage
					if ((read_byte & 0x20) == 0)
					{
						// bit 3: effect 0 present
						// bit 4: effect value 0 present
						if ((read_byte & 0x8) && (read_byte & 0x10))
						{
							data.effects.push(file.readUInt8(pattern_data_pointer));
							pattern_data_pointer++;
							data.effects.push(file.readUInt8(pattern_data_pointer));
							pattern_data_pointer++;
						}
						else if (read_byte & 0x8)
						{
							data.effects.push(file.readUInt8(pattern_data_pointer));
							pattern_data_pointer++;

							data.effects.push(-1);
						}
						else if (read_byte & 0x10)
						{
							data.effects.push(-1);

							data.effects.push(file.readUInt8(pattern_data_pointer));
							pattern_data_pointer++;
						}
					}

					// get effect data
					for (let j = 0; j < extra_effect_byte_count; j++)
					{
						// byte says which extra effects are in use
						let fx_byte = effect_bytes[j];

						// parse them in pairs
						for (let k = 0; k < 8; k += 2)
						{
							if ((fx_byte & 0x3) == 0x3)
							{
								data.effects.push(file.readUInt8(pattern_data_pointer));
								pattern_data_pointer++;
								data.effects.push(file.readUInt8(pattern_data_pointer));
								pattern_data_pointer++;
							}
							else if (fx_byte & 0x1)
							{
								data.effects.push(file.readUInt8(pattern_data_pointer));
								pattern_data_pointer++;

								data.effects.push(-1);
							}
							else if (fx_byte & 0x2)
							{
								data.effects.push(-1);

								data.effects.push(file.readUInt8(pattern_data_pointer));
								pattern_data_pointer++;
							}

							fx_byte = fx_byte >> 2;
						}
					}

					pattern.data.push(data);
				}

				//pattern.data_bytes = new Uint8Array(file.subarray(data_start_pointer, pattern_data_pointer + 1));
			}

			// fill in rest of pattern
			while (pattern.data.length < song.pattern_length)
			{
				pattern.data.push({
					note: 0,
					octave: 0,
					instrument: -1,
					volume: -1,
					effects: []
				});
			}

			// add to patterns list
			song.patterns[pattern.channel][pattern.index] = pattern;
		}

	}

	// load instruments
	for (let i = 0; i < song.instrument_count; i++)
	{
		let instrument_pointer = song.instrument_pointers[i];
		
		let instrument = {
			name: "",
			type: file.readUInt16LE(instrument_pointer + 10),
			size: file.readUInt32LE(instrument_pointer + 4),
			macros: [],
			features: [],
		};
		
		// go through features and add them to the instrument
		let feature_pointer = instrument_pointer + 12;
		
		while (feature_pointer < instrument_pointer + instrument.size)
		{
			let feature = {
				code: file.toString("utf8", feature_pointer, feature_pointer + 2),
				length: file.readUInt16LE(feature_pointer + 2),
				data: [],
			}
			
			// move pointer past header
			feature_pointer += 4;
			
			// Instrument name feature
			if (feature.code == "NA")
			{
				instrument.name = file.toString("utf8", feature_pointer, feature_pointer + feature.length - 1);
			}
			// End features if we reach this code
			else if (feature.code == "EN")
			{
				break;
			}
			// OPL drums 
			else if (feature.code == "LD")
			{
				instrument.opl_drums = {
					fixed_freq: file.readUInt8(feature_pointer),
					kick_freq: file.readUInt16LE(feature_pointer + 1),
					snare_hat_freq: file.readUInt16LE(feature_pointer + 3),
					tom_top_freq: file.readUInt16LE(feature_pointer + 5),
				}
			}
			// Macros
			else if (feature.code == "MA")
			{				
				let header_length = file.readUInt16LE(feature_pointer);
				let macro_pointer = feature_pointer + 2;
				
				while (macro_pointer < feature_pointer + feature.length)
				{
					let macro = {
						code: file.readUInt8(macro_pointer),
						length: file.readUInt8(macro_pointer + 1),						
						loop: file.readUInt8(macro_pointer + 2),
						release: file.readUInt8(macro_pointer + 3),
						mode: file.readUInt8(macro_pointer + 4),
						word_size: macro_word_size_convert[(file.readUInt8(macro_pointer + 5) >> 6) & 0x3],
						type: (file.readUInt8(feature_pointer + 5) >> 1) & 0x3,
						delay: file.readUInt8(feature_pointer + 6),
						speed: file.readUInt8(feature_pointer + 7),
						data: []
					}
					
					// end of macros
					if (macro.code == 255)
					{
						break;
					}
					
					// advance pointer to macro data
					macro_pointer += header_length;
					
					// get macro data
					for (let j = 0; j < macro.length; j++)
					{
						macro.data.push(
							file[macro_word_read_function[macro.word_size]](macro_pointer + (j * macro.word_size))
						);
					}
					
					// add macro to list
					if (macro.code == 0)
					{
						instrument.volume_macro = macro;
					}
					else
					{
						instrument.macros.push(macro);
					}
					
					// advance pointer
					macro_pointer += macro.length * macro.word_size;
				}
			}
			// FM patch
			else if (feature.code == "FM")
			{
				let fm_pointer = feature_pointer;
				
				instrument.fm = {
					op_count: file.readUInt8(fm_pointer) & 0xf,
					op_enabled: file.readUInt8(fm_pointer) >> 4,
					
					feedback: file.readUInt8(fm_pointer + 1) & 0x7,
					algorithm: (file.readUInt8(fm_pointer + 1) >> 4) & 0x7,
					
					fms: file.readUInt8(fm_pointer + 2) & 0x7,
					ams: (file.readUInt8(fm_pointer + 2) >> 3) & 0x3,
					fms2: (file.readUInt8(fm_pointer + 2) >> 5) & 0x7,
					
					opll_patch: file.readUInt8(fm_pointer + 3) & 0x1f,
					am2: (file.readUInt8(fm_pointer + 3) >> 6) & 0x3,
					
					operator_data: [],
				};
				
				let operator_pointer = feature_pointer + 4;
				
				// get operator data for each operator
				for (let j = 0; j < instrument.fm.op_count; j++)
				{
					let operator = {
						mult: file.readUInt8(operator_pointer) & 0xf,
						dt: (file.readUInt8(operator_pointer) >> 4) & 0x7,
						ksr: file.readUInt8(operator_pointer) >> 7,
						
						tl: file.readUInt8(operator_pointer + 1) & 0x7f,
						sus: file.readUInt8(operator_pointer + 1) >> 7,
						
						ar: file.readUInt8(operator_pointer + 2) & 0x1f,
						vib: (file.readUInt8(operator_pointer + 2) >> 5) & 0x1,
						rs: (file.readUInt8(operator_pointer + 2) >> 6) & 0x3,
						
						dr: file.readUInt8(operator_pointer + 3) & 0x1f,
						ksl: (file.readUInt8(operator_pointer + 3) >> 5) & 0x3,
						am: file.readUInt8(operator_pointer + 3) >> 7,
						
						d2r: file.readUInt8(operator_pointer + 4) & 0x1f,
						kvs: (file.readUInt8(operator_pointer + 4) >> 5) & 0x3,
						egt: file.readUInt8(operator_pointer + 4) >> 7,
						
						rr: file.readUInt8(operator_pointer + 5) & 0xf,
						sl: file.readUInt8(operator_pointer + 5) >> 4,
						
						ssg: file.readUInt8(operator_pointer + 6) & 0xf,
						dvb: file.readUInt8(operator_pointer + 6) >> 4,
						
						ws: file.readUInt8(operator_pointer + 7) & 0x7,
						dt2: (file.readUInt8(operator_pointer + 7) >> 3) & 0x3,
						dam: (file.readUInt8(operator_pointer + 7) >> 5) & 0x7,
					};
										
					instrument.fm.operator_data.push(operator);
										
					operator_pointer += 8;
				}
			}
			else
			{	
				feature.data = file.subarray(feature_pointer, feature_pointer + feature.length);
				feature.data = [...feature.data];
				
				instrument.features.push(feature);
			}
			
			// advance pointer
			feature_pointer += feature.length;
		}
		
		// add instrument
		song.instruments[i] = instrument;
	}
	
	// console.log(song.patterns[0][0].data);
	await fsPromises.writeFile(output_file, JSON.stringify(song, null, "\t"));
}

main(process.argv[2], process.argv[3]);
