/*
	Joe Kennedy 2024
	Create asm include of SN76489 fnums
*/

const fs = require('fs');	 
const note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];

const MUSIC_CLOCK = 3579545.0;

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
	sn_fnums.push(MUSIC_CLOCK / ( 2 * 16 * midi_note_frequencies[i]));
}

var output = [];

for (var i = 45; i < 126; i++)
{
	var out;
	var note_name = note_names[i % 12];
	note_name = "midi: " + note_name + (Math.floor(i / 12) - 1) + ", fur: " + note_name + (Math.floor(i / 12) - 3);
	
	output.push({fnum: Math.round(sn_fnums[i]), note_name: note_name});
}

// output an include file with the sn note data
let outfile = fs.openSync("fnums_sn.inc", "w+");

fs.writeSync(outfile, "sn_tone_lookup:\n");

for (var i = 0; i < output.length; i++)
{
	fs.writeSync(outfile, "; " + output[i].note_name + "\n");
	fs.writeSync(outfile, ".dw " + output[i].fnum + "\n");
}

fs.closeSync(outfile);	   
