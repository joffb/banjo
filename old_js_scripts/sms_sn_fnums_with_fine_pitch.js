/*
	Joe Kennedy 2024
	Create asm include of SN76489 fnums and fine pitch offsets
*/

const fs = require('fs');	 
var note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];

// midi note frequencies for all midi notes

var midi_note_frequencies = [];

for (var i = 0; i < 128; i++)
{
	midi_note_frequencies.push(Math.pow(2.0, (i - 69.0)/12.0) * 440.0);
}


// fnum values for the sn chip for the given frequencies

var sn_fnums = [];

for (var i = 0; i < 128; i++)
{
	sn_fnums.push(3579545.0 / ( 2 * 16 * midi_note_frequencies[i]));
}

/*
	prepare data for the playable range of notes
	
	range is midi A2 - F9, furnace A0 to F7
	total of 80 notes from midi note numbers 45 to 125

	furnace seems to use 32 steps between notes for the fine pitch bend/slide
	rather than storing a byte for all 32 steps, we can store 5 bytes
	each byte will have a power of two division of the difference between fnums
	these will correspond to a bit in the fine pitch amount
	
		e.g. if the difference between fnums is 30 the bytes will thus contain
			
				[30/32, 30/16, 30/8, 30/4, 30/2]
				[0, 1, 3, 7, 15]			
	
	fnum is 2 bytes, and each component of the diff is 1 byte, for a total of 7 bytes
	add 1 byte of padding to get to 8 bytes for easy access
	8 * 80 = 640 byte table

	to get the note in between two notes we then:
	
	1) get the starting fnum
	2) look at each bit of the fine pitch (which will be 0-31) and sum the diff value corresponding to each bit
	
		e.g. going from A2 to A#2
										/32	/16	/8	/4	/2
				A2	fnum: 1017	diffs:	2, 	4, 	7, 	14,	29 	(full diff: 57)
				A#2	fnum: 960	diffs:	2,	3,	7,	14,	27	(full diff: 54)
				
				if the fine pitch is 23, it's 10111 in binary, which corresponds to
				
					1	1	1	0	1
					2	4	7	14 	29
					
					2 + 4 + 7 + 	29
					
				which totals 42
				
	3) subtract the sum of those from the fnum (we subtracting because the fnum gets smaller as the pitch gets higher!)
	
				1017 - (2 + 4 + 7 + 29) = 975
				
*/

var output = [];

for (var i = 45; i < 126; i++)
{
	var out;
	var note_name = note_names[i % 12];
	note_name = "midi: " + note_name + (Math.floor(i / 12) - 1) + ", fur: " + note_name + (Math.floor(i / 12) - 3);
	
	var diff = Math.round(sn_fnums[i]) - Math.round(sn_fnums[i + 1]);
	
	// more than 1 difference between fnums
	if (diff > 1)
	{
		out = [
			Math.round(sn_fnums[i]), 
			Math.round(diff/32),
			Math.round(diff/16), 
			Math.round(diff/8), 
			Math.round(diff/4), 
			Math.round(diff/2),
			note_name,
		];
	}
	// 1 or less difference between fnums, make it so there's no step between notes
	// at these sorts of fnums, pitches are pretty out of tune and there's barely any pitch resolution
	else
	{
		out = [
			Math.round(sn_fnums[i]), 
			0,
			0, 
			0, 
			0, 
			0,
			note_name,
		];
	}
	
	// this should be very close or equal to the next fnum
	// out[6] = out[0] - (out[1] + out[2] + out[3] + out[4] + out[5]);
	
	output.push(out);
}

// output an include file with the sn note data
let outfile = fs.openSync("sn_fnums.inc", "w+");

for (var i = 0; i < output.length; i++)
{
	fs.writeSync(outfile, "; " + output[i][6] + "(" + output[i][0] + ")\n");
	fs.writeSync(outfile, "; fnum " + output[i][0] + "\n");
	fs.writeSync(outfile, ".db " + (output[i][0] & 0xf) + ", " + ((output[i][0] >> 4) & 0x3f) + "\n");
	
	fs.writeSync(outfile, ".db ");
	
	for (var j = 1; j < 6; j ++)
	{
		fs.writeSync(outfile, output[i][j] + ", ");
	}
	
	fs.writeSync(outfile, "0\n");
}

fs.closeSync(outfile);	   
