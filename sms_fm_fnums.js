/*
	Joe Kennedy 2024
	Create asm include of OPLL fnums
*/

const fs = require('fs');	 
const note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"];

const MUSIC_CLOCK = 3579545.0;
const MUSIC_FM_FSAM = (MUSIC_CLOCK / 72.0);

// midi note frequencies for all midi notes

var notes = [];

for (var oct = 0; oct < 8; oct++)
{
    for (var i = 0; i < 12; i++)
    {
        // frequency of note at octave 4
        let freq = Math.pow(2.0, i/12.0) * 261.63;
        
        // f number of note at octave 4
        let fnum = Math.floor(((freq * Math.pow(2.0, 18)) / MUSIC_FM_FSAM) / Math.pow(2, 3))

        // combine with octave
        fnum = fnum | (oct << 9);
        
        notes.push({
            note_name: (note_names[i] + oct), 
            freq: freq, 
            fnum: fnum
        });
    }
}

// output an include file with the sn note data
let outfile = fs.openSync("fnums_fm.inc", "w+");

fs.writeSync(outfile, "fm_tone_lookup:\n");
fs.writeSync(outfile, ".ifdef INCLUDE_OPLL\n");

for (var i = 0; i < notes.length; i++)
{
	fs.writeSync(outfile, "; " + notes[i].note_name + "\n");
	fs.writeSync(outfile, ".dw " + notes[i].fnum + "\n");
}

fs.writeSync(outfile, ".endif\n");

fs.closeSync(outfile);	   
