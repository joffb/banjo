/*
	Joe Kennedy 2024
	Create asm include of AY-3-8910 fnums
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


// fnum values for the ay chip for the given frequencies

var sn_fnums = [];

for (var i = 0; i < 128; i++)
{
	var freq = MUSIC_CLOCK / ( 2 * 8 * midi_note_frequencies[i]);

	sn_fnums.push(freq > 4095 ? 4095 : freq);
}

var output = [];

for (var i = 24; i < 128; i++)
{
	var out;
	var note_name = note_names[i % 12];
	note_name = "midi: " + note_name + (Math.floor(i / 12) - 1) + ", fur: " + note_name + (Math.floor(i / 12) - 2);
	
	output.push({fnum: Math.round(sn_fnums[i]) & 4095, note_name: note_name});
}

// output an include file with the ay note data
let outfile = fs.openSync("fnums_ay.inc", "w+");

fs.writeSync(outfile, "ay_tone_lookup:\n");

for (var i = 0; i < output.length; i++)
{
	fs.writeSync(outfile, "; " + output[i].note_name + "\n");
	fs.writeSync(outfile, ".dw " + output[i].fnum + "\n");
}

fs.closeSync(outfile);	   
