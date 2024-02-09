const fs = require('fs');
const fsPromises = fs.promises;
const {EOL} = require('os');

if (process.argv[2] == "sfx")
{
	main(process.argv[4], process.argv[5],  process.argv[6], true, parseInt(process.argv[3], 10));
}
else
{
	main(process.argv[2], process.argv[3], process.argv[4]);
}

const NOTE_ON 			= 0x00;
const NOTE_OFF 			= 0x01;
const INSTRUMENT_CHANGE	= 0x02;
const FM_PATCH 			= 0x03;
const FM_DRUM 			= 0x04;
const SN_NOISE_MODE		= 0x05;
const PITCH_SLIDE_UP	= 0x06;
const PITCH_SLIDE_DOWN	= 0x07;
const ARPEGGIO			= 0x08;
const PORTAMENTO		= 0x09;
const LINE_WAIT			= 0x0a;
const END_PATTERN_LINE	= 0xff;

const INSTRUMENT_SIZE	= 16;
const FM_PATCH_SIZE		= 8;

const CHIP_SN76489 = 0x03;		// 0x03: SMS (SN76489) - 4 channels
const CHIP_SN76489_OPLL = 0x43;	// 0x43: SMS (SN76489) + OPLL (YM2413) - 13 channels (compound!)
const CHIP_OPLL = 0x89;			// 0x89: OPLL (YM2413) - 9 channels
const CHIP_OPLL_DRUMS = 0xa7;	// 0xa7: OPLL drums (YM2413) - 11 channels

const CHAN_SN76489 = 0x00
const CHAN_OPLL = 0x01
const CHAN_OPLL_DRUMS = 0x02

// whether the drum channel needs its volume to be pre-shifted
const pre_shift_fm_drum_value = [0,0,4,0,4];

// load the json 
async function load_file(filename)
{
	// file doesn't exist
	if (!fs.existsSync(filename))
	{
		return false;
	}
	
	let file = await fsPromises.readFile(filename);

	try {
		let json = JSON.parse(file);
		return json;
	}
	catch (e)
	{
		return false;
	}
}


async function main (song_prefix, input_file, output_file, sfx, sfx_channel)
{
	let song = await load_file(input_file);
	
	if (!song)
	{
		console.log("Not a valid json file");
		return;
	}
	
	// flags to say which chips are active
	song.has_sn = false;
	song.has_fm = false;
	song.has_fm_drums = false;

	if (song.sound_chips.indexOf(CHIP_SN76489) != -1 || song.sound_chips.indexOf(CHIP_SN76489_OPLL) != -1)
	{
		song.has_sn = true;
	}
	
	if (song.sound_chips.indexOf(CHIP_OPLL) != -1 || song.sound_chips.indexOf(CHIP_SN76489_OPLL) != -1)
	{
		song.has_fm = true;
	}

	if (song.sound_chips.indexOf(CHIP_OPLL_DRUMS) != -1)
	{
		song.has_fm_drums = true;
	}	
	
	let output = [];
		
	// channel types and channel numbers
	let sn_count = 0;
	let channel_types = [];
	
	for (var i = 0; i < song.sound_chips.length; i++)
	{
		let sound_chip = song.sound_chips[i];
		
		if (sound_chip == CHIP_SN76489)
		{
			for (let i = 0; i < 4; i++)
			{
				channel_types.push(CHAN_SN76489);
				channel_types.push(i | (sn_count << 4));
			}

			sn_count++;
		}

		else if (sound_chip == CHIP_OPLL)
		{
			for (let i = 0; i < 9; i++)
			{
				channel_types.push(CHAN_OPLL);
				channel_types.push(i);
			}
		}

		else if (sound_chip == CHIP_OPLL_DRUMS)
		{
			for (let i = 0; i < 6; i++)
			{
				channel_types.push(CHAN_OPLL);
				channel_types.push(i);
			}
			
			for (let i = 0; i < 5; i++)
			{
				channel_types.push(CHAN_OPLL_DRUMS);
				channel_types.push(i);
			}
		}

		else if (sound_chip == CHIP_SN76489_OPLL)
		{
			for (let i = 0; i < 4; i++)
			{
				channel_types.push(CHAN_SN76489);
				channel_types.push(i | (sn_count << 4));
			}
			for (let i = 0; i < 9; i++)
			{
				channel_types.push(CHAN_OPLL);
				channel_types.push(i);
			}

			sn_count++;
		}
	}

	// in sfx mode, only include channel_types for the specified sfx channel
	if (sfx)
	{
		let new_channel_types = [];
		
		new_channel_types[0] = channel_types[(sfx_channel * 2)];
		new_channel_types[1] = channel_types[(sfx_channel * 2) + 1];
		
		channel_types = new_channel_types;
	}
	
	// fill out channel types array
	while (channel_types.length < 32)
	{
		channel_types.push(0xff);
	}

	
	//
	// process instruments
	//
	
	let instrument_offsets = [0x0000];
	let instruments = [];
	
	let volume_macro_offsets = [0x0000];
	let volume_macros = [];
	
	for (let i = 0; i < song.instrument_count; i++)
	{
		let instrument = song.instruments[i];
		let instrument_bin = [];
		
		// organise volume macro data
		if (instrument.hasOwnProperty("volume_macro"))
		{
			instrument_bin.push(instrument.volume_macro.length);
			instrument_bin.push(instrument.volume_macro.mode);
			instrument_bin.push(instrument.volume_macro.loop);
			
			let volume_macro_offset = volume_macro_offsets[volume_macro_offsets.length - 1];
			
			instrument_bin.push(volume_macro_offset);
			
			// invert volume data so 0 is loud and 15 is silent
			for (let j = 0; j < instrument.volume_macro.data.length; j++)
			{
				instrument.volume_macro.data[j] = 15 - instrument.volume_macro.data[j];
			}
			
			// add macro data to separate arrays and set up offset of next volume macro (if there is one)
			volume_macros.push(instrument.volume_macro.data);
			volume_macro_offsets.push(volume_macro_offset + instrument.volume_macro.length);
		}
		
		// no volume_macro
		else
		{
			instrument_bin.push(0);
			instrument_bin.push(0);
			instrument_bin.push(0);
			instrument_bin.push(0xffff);
		}
		
		// FM preset change (only for OPLL FM instruments)
		if (instrument.type == 13 && instrument.hasOwnProperty("fm"))
		{			
			// custom fm patch data
			if (instrument.fm.opll_patch == 0)
			{
				// pre-shift the patch number into the upper nibble
				instrument_bin.push((instrument.fm.opll_patch & 0xf) << 4);

				let fm_patch = [];
				let operator = null;

				for (let j = 0; j < 2; j++)
				{
					operator = instrument.fm.operator_data[j];
					
					// operator sustain seems to be in top bit of ssg in furnace format
					fm_patch[0 + j] = operator.mult | (operator.ksr << 4) | (((operator.ssg >> 3) & 0x1) << 5) | (operator.vib << 6) | (operator.am << 7);
				}
				
				operator = instrument.fm.operator_data[0];
				fm_patch[2] = operator.tl | (operator.ksl << 6);
				
				// op 1 and 2 half-sine modes seem to be in instrument's fms and ams in furnace format
				operator = instrument.fm.operator_data[1];
				fm_patch[3] = (instrument.fm.feedback) | (instrument.fm.ams << 3) | (instrument.fm.fms << 4) | (operator.ksl << 6);
				
				for (let j = 0; j < 2; j++)
				{
					operator = instrument.fm.operator_data[j];
					fm_patch[4 + j] = operator.dr | (operator.ar << 4);
					fm_patch[6 + j] = operator.rr | (operator.sl << 4);
				}
				
				for (let j = 0; j < 8; j++)
				{
					instrument_bin.push(fm_patch[j]);
				}
			}

			// custom drum patch with fixed pitch
			else if (instrument.fm.opll_patch == 16)
			{
				// pre-shift the patch number into the upper nibble
				instrument_bin.push(instrument.opl_drums.fixed_freq ? 0xff : 0);

				instrument_bin.push(instrument.opl_drums.kick_freq & 0xff);
				instrument_bin.push(instrument.opl_drums.kick_freq >> 8);
				
				instrument_bin.push(instrument.opl_drums.snare_hat_freq & 0xff);
				instrument_bin.push(instrument.opl_drums.snare_hat_freq >> 8);

				instrument_bin.push(instrument.opl_drums.tom_top_freq & 0xff);
				instrument_bin.push(instrument.opl_drums.tom_top_freq >> 8);

				instrument_bin.push(0xff);
				instrument_bin.push(0xff);
			}
			
			// using a standard preset so no extra fm data
			else
			{
				// pre-shift the patch number into the upper nibble
				instrument_bin.push((instrument.fm.opll_patch & 0xf) << 4);

				for (let j = 0; j < 8; j++)
				{
					instrument_bin.push(0xff);
				}
			}
		}
		// not an FM instrument
		else
		{
			instrument_bin.push(0xff);
			
			for (let j = 0; j < 8; j++)
			{
				instrument_bin.push(0xff);
			}
		}
			
		instruments.push(instrument_bin);
		instrument_offsets.push(instrument_offsets[i] + INSTRUMENT_SIZE)
	}
	
	let patterns = [];
	let pattern_offsets = [0x0000];
	let pattern_offsets_by_index = {};
	let pattern_channel_offsets = [];
	
	// process patterns
	// go through each channel
	for (let i = 0; i < song.channel_count; i++)
	{
		// in sfx mode, only process sfx channel
		if (sfx && i != sfx_channel)
		{
			continue;
		}
		
		let channel_type = channel_types[i * 2];
		let channel_subchannel = channel_types[(i * 2) + 1];
		
		// only one channel for sfx
		if (sfx)
		{
			channel_type = channel_types[0];
			channel_subchannel = channel_types[1]
		}
		
		// separate pattern arrays per channel
		patterns[i] = [];
		
		// start of this channel's patterns
		pattern_channel_offsets.push(pattern_offsets[pattern_offsets.length - 1]);

		let current_inst = null;
		let current_vol = 15;

		// go through each pattern in the channel
		for (let j = 0; j < song.patterns[i].length; j++)
		{
			let pattern = song.patterns[i][j];
			
			let pattern_bin = [];
			
			if (pattern)
			{
				// go through every line
				for (let k = 0; k < song.pattern_length; k++)
				{
					let line = pattern.data[k];
					
					// instrument has changed
					if (current_inst != line.instrument && line.instrument != -1)
					{
						current_inst = line.instrument;

						if (channel_type == CHAN_OPLL_DRUMS)
						{
							pattern_bin.push("FM_DRUM")
							pattern_bin.push(current_inst);
						}
						else
						{
							pattern_bin.push("INSTRUMENT_CHANGE")
							pattern_bin.push(current_inst);
						}
						
					}
					
					// check effects
					for (let eff = 0; eff < line.effects.length; eff += 2)
					{
						// SN noise mode
						if (line.effects[eff] == 0x20)
						{
							pattern_bin.push("SN_NOISE_MODE");
							
							let noise_mode = (line.effects[eff + 1] & 0x1) << 2;
							let noise_freq = (line.effects[eff + 1] & 0x10) ? 0x3 : 0x0;
							
							pattern_bin.push(0x80 | (0x3 << 5) | noise_mode | noise_freq);
						}
						
						// Arpeggio
						else if (line.effects[eff] == 0x00)
						{
							pattern_bin.push("ARPEGGIO");
							pattern_bin.push(line.effects[eff + 1] >> 4);
							pattern_bin.push(line.effects[eff + 1] & 0xf);
						}

						// Pitch slide up
						else if (line.effects[eff] == 0x01)
						{
							pattern_bin.push("PITCH_SLIDE_UP");
							pattern_bin.push(line.effects[eff + 1]);
						}
						
						// Pitch slide down
						else if (line.effects[eff] == 0x02)
						{
							pattern_bin.push("PITCH_SLIDE_DOWN");
							pattern_bin.push(line.effects[eff + 1]);
						}

						// Portamento
						else if (line.effects[eff] == 0x03)
						{
							pattern_bin.push("PORTAMENTO");
							pattern_bin.push(line.effects[eff + 1]);
						}
					}
					
					
					// empty
					if (line.note == 0 && line.octave == 0)
					{
						
					}
					// note on
					else if (line.note <= 12)
					{
						pattern_bin.push("NOTE_ON");
						
						let volume = (line.volume == -1) ? current_vol : line.volume;
						
						// sn channel, this byte can be sent straight to the output to set the volume
						if (channel_type == CHAN_SN76489)
						{
							pattern_bin.push((15 - volume) | 0x80 | 0x10 | ((channel_subchannel & 0xf) << 5))
						}
						else if (channel_type == CHAN_OPLL)
						{
							pattern_bin.push((15 - volume))
						}
						else if (channel_type == CHAN_OPLL_DRUMS)
						{
							pattern_bin.push((15 - volume) << pre_shift_fm_drum_value[channel_subchannel]);
						}
					
						// note number
						if (channel_type == CHAN_SN76489)
						{
							pattern_bin.push((line.note + (line.octave * 12) + 36 - 45) & 0x7f);
						}
						else
						{
							pattern_bin.push((line.note + (line.octave * 12)) & 0x7f);
						}
					
						if (line.volume != -1)
						{
							current_vol = line.volume;
						}
					}
					// note off
					else if (line.note == 100)
					{
						pattern_bin.push("NOTE_OFF");
					}
					
					// end line marker
					pattern_bin.push(END_PATTERN_LINE);
				}

				//
				let run_length = 0;
				let run_start = -1;
				let pattern_bin_rle = [];

				// when a run of END_PATTERN_LINE ends, replace it if it's long enough
				function run_end()
				{
					if (run_length > 2)
					{
						pattern_bin_rle.push("LINE_WAIT");
						pattern_bin_rle.push(run_length - 1);
					}
					else
					{
						for (let k = 0; k < run_length; k++)
						{
							pattern_bin_rle.push(END_PATTERN_LINE);
						}
					}

					run_length = 0;
					run_start = -1;
				}

				// look through pattern_bin for runs of END_PATTERN_LINE
				for (let k = 0; k < pattern_bin.length; k++)
				{
					if (pattern_bin[k] == END_PATTERN_LINE)
					{
						if (run_start == -1)
						{
							run_start = k;
						}

						run_length++;
					}
					else
					{
						run_end();
						pattern_bin_rle.push(pattern_bin[k]);
					}
				}

				run_end();
				
				pattern_bin = pattern_bin_rle;

				// add to patterns array
				patterns[i].push(pattern_bin);
				
				// create new pattern offset
				let pattern_offset = pattern_offsets[pattern_offsets.length - 1];
				pattern_offsets.push(pattern_offset + pattern_bin.length);
				
				// store pattern offset indexed by channel and pattern index
				pattern_offsets_by_index[i + "_" + pattern.index] = pattern_offset;
			}
		}
	}

	// write orders as pointers to their respective patterns
	let orders = [];
	let order_offsets = [0x0000];
	let order_channel_offsets = [];
	
	for (let i = 0; i < song.channel_count; i++)
	{
		// in sfx mode, only process sfx channel
		if (sfx && i != sfx_channel)
		{
			continue;
		}
		
		// array per channel
		orders[i] = [];
		
		// offset for the order data for this channel
		order_channel_offsets.push(order_offsets[order_offsets.length - 1]);
		
		for (let j = 0; j < song.orders_length; j++)
		{
			let order_pattern_index = i + "_" + song.orders[i][j];
			orders[i].push(pattern_offsets_by_index[order_pattern_index]);
			
			// add next offset of next order
			order_offsets.push(order_offsets[order_offsets.length - 1] + 2);
		}
	}

	// output asm file
	let outfile = fs.openSync(output_file, "w+");
	
	function writelabel(label)
	{
		fs.writeSync(outfile, song_prefix + "_" + label + ":" + EOL);
	}
	
	// initial label
	fs.writeSync(outfile, song_prefix + ":" + EOL);
	
	// basic song data
	writelabel("channel_count");
	fs.writeSync(outfile, ".db " + (sfx ? 1 : song.channel_count) + EOL);
	writelabel("is_sfx");
	fs.writeSync(outfile, ".db " + (sfx ? 1 : 0) + EOL);
	writelabel("sfx_channel");
	fs.writeSync(outfile, ".db " + (sfx ? sfx_channel : 0xff) + EOL);
	writelabel("has_sn");
	fs.writeSync(outfile, ".db " + (song.has_sn ? 1 : 0) + EOL);
	writelabel("has_fm");
	fs.writeSync(outfile, ".db " + (song.has_fm ? 1 : 0) + EOL);
	writelabel("has_fm_drums");
	fs.writeSync(outfile, ".db " + (song.has_fm_drums ? 1 : 0) + EOL);
	writelabel("time_base");
	fs.writeSync(outfile, ".db " + (song.time_base + 1) + EOL);
	writelabel("speed_1");
	fs.writeSync(outfile, ".db " + (song.speed_1) + EOL);
	writelabel("speed_2");
	fs.writeSync(outfile, ".db " + song.speed_2 + EOL);
	writelabel("pattern_length");
	fs.writeSync(outfile, ".db " + song.pattern_length + EOL);
	writelabel("orders_length");
	fs.writeSync(outfile, ".db " + song.orders_length + EOL);
	writelabel("instrument_ptrs");
	fs.writeSync(outfile, ".dw " + song_prefix + "_instrument_pointers" + EOL);
	writelabel("order_ptrs");
	fs.writeSync(outfile, ".dw " + song_prefix + "_order_pointers" + EOL);
	
	writelabel("channel_types");
	fs.writeSync(outfile, ".db " + channel_types.join(", ") + EOL);
	
	fs.writeSync(outfile, EOL + EOL);

	// volume macros
	writelabel("volume_macros");
	
	for (let i = 0; i < volume_macros.length; i++)
	{
		writelabel("volume_macro_" + i);
		
		fs.writeSync(outfile, ".db " + volume_macros[i].join(", ") + EOL);
	}

	// instruments
	writelabel("instrument_pointers");
	
	for (let i = 0; i < instruments.length; i++)
	{
		fs.writeSync(outfile, ".dw " + song_prefix + "_instrument_" + i + EOL);
	}
	
	fs.writeSync(outfile, EOL + EOL);

	// instruments
	writelabel("instrument_data");
	
	for (let i = 0; i < instruments.length; i++)
	{
		let instrument = instruments[i];
		
		writelabel("instrument_" + i);
		fs.writeSync(outfile, "\t.db " + instrument[0] + ", " + instrument[1] + ", " + instrument[2] + EOL);
		
		// volume macro pointer
		if (instrument[3] != 0xffff)
		{
			fs.writeSync(outfile, "\t.dw " + song_prefix + "_volume_macros + " + instrument[3] + EOL);
		}
		else
		{
			fs.writeSync(outfile, "\t.dw 0xffff" + EOL);
		}
		
		// fm preset
		fs.writeSync(outfile, "\t.db " + instrument[4] + EOL);
		
		// fm patch
		fs.writeSync(outfile, "\t.db " + instrument.slice(5, 13).join(", ") + EOL);	
	}
	
	fs.writeSync(outfile, EOL + EOL);
	
	
	// order pointers
	writelabel("order_pointers");
	
	for (let i = 0; i < song.channel_count; i++)
	{
		// in sfx mode, only process sfx channel
		if (sfx && i != sfx_channel)
		{
			continue;
		}
		
		fs.writeSync(outfile, "\t.dw " + song_prefix + "_orders_channel_" + i + EOL);
	}

	// orders
	writelabel("orders");
	for (let i = 0; i < song.channel_count; i++)
	{
		// in sfx mode, only process sfx channel
		if (sfx && i != sfx_channel)
		{
			continue;
		}
		
		writelabel("orders_channel_" + i);		
		
		for (let j = 0; j < orders[i].length; j++)
		{	
			if (j % 4 == 0)
			{
				fs.writeSync(outfile, EOL + "\t.dw ");
			}
				
			fs.writeSync(outfile, song_prefix + "_patterns + 0x" + orders[i][j].toString(16));

			if ((j % 4 != 3) && (j != orders[i].length - 1))
			{
				fs.writeSync(outfile, ", ");
			}
		}
		
		fs.writeSync(outfile, EOL);
	}
	
	fs.writeSync(outfile, EOL + EOL);
	
	// patterns
	writelabel("patterns");
	
	for (let i = 0; i < song.channel_count; i++)
	{
		// in sfx mode, only process sfx channel
		if (sfx && i != sfx_channel)
		{
			continue;
		}
		
		writelabel("patterns_channel_" + i);
		
		for (let j = 0; j < patterns[i].length; j++)
		{
			fs.writeSync(outfile, ".db " +  patterns[i][j].join(", ") + EOL);
		}
	}
	
	// close asm file
	fs.closeSync(outfile);
}	