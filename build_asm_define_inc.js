//
// Builds defines_sdas.inc and defines_wladx.inc for the different asm dialects
// 
// Joe Kennedy 2024
//

const fs = require('fs');

var defines = [
    [
        { name: "NOTE_ON", value: 0x00 },
        { name: "NOTE_OFF",	value: 0x01 },
        { name: "INSTRUMENT_CHANGE", value: 0x02 },
        { name: "FM_PATCH", value: 0x03 },
        { name: "FM_DRUM", value: 0x04 },
        { name: "SN_NOISE_MODE", value: 0x05 },
        { name: "PITCH_SLIDE_UP", value: 0x06 },
        { name: "PITCH_SLIDE_DOWN", value: 0x07 },
        { name: "ARPEGGIO", value: 0x08 },
        { name: "PORTAMENTO", value: 0x09 },
        { name: "LINE_WAIT", value: 0x0a },
        { name: "END_PATTERN_LINE", value: 0xff },
    ],
    [
        { name: "SN76489_PORT", value: 0x7f },
        { name: "SN76489_2_PORT", value: 0x7b },
        { name: "OPLL_REG_PORT", value: 0xf0 },
        { name: "OPLL_DATA_PORT", value: 0xf1  },
    ],
    [
        { name: "AUDIO_CONTROL_PORT", value: 0xf2 },
    ],
    [
        { name: "CHAN_SN76489", value: 0x00 },
        { name: "CHAN_OPLL", value: 0x01 },
        { name: "CHAN_OPLL_DRUMS", value: 0x02 },
    ],
    [
        { name: "SLIDE_TYPE_NONE", value: 0x00 },
        { name: "SLIDE_TYPE_UP", value: 0x01 },
        { name: "SLIDE_TYPE_DOWN", value: 0x02 },
        { name: "SLIDE_TYPE_PORTA", value: 0x03 },
    ],
    [
        { name: "CHAN_FLAG_MUTED", value: 0x01 },
        { name: "CHAN_FLAG_NOTE_ON", value: 0x02 },
        { name: "CHAN_FLAG_LEGATO", value: 0x04 },
        { name: "CHAN_FLAG_PITCH_CHANGED", value: 0x08 },
    ],
    [
        { name: "BANJO_MAGIC_BYTE", value: 0xba },
    ],
    [
        { name: "SLOT_2_BANK_CHANGE", value: 0xffff },
    ]
];

var structs = [
    {
        name: "instrument",
        members: [
            { name: "volume_macro_len", size: "db" },
            { name: "volume_macro_mode", size: "db" },
            { name: "volume_macro_loop", size: "db" },
            { name: "volume_macro_ptr", size: "dw" },
            
            { name: "fm_preset", size: "db" },
            { name: "fm_patch", size: 8 },
            
            { name: "padding", size: 2 },
        ]
    },
    {
        name: "channel",
        members: [
            { name: "flags", size: "db", comment: "1: muted - 2: note on - 4: legato - 8: pitch changed" },
            //{ name: "pitch_changed", size: "db", comment: "flag whether the pitch needs to be recalculated"},
            //{ name: "legato", size: "db", comment: "if legato, don't retrigger envelopes on a note_on" },
            //{ name: "note_on", size: "db" },

            { name: "type", size: "db", comment: "type of chip"},
            { name: "subchannel", size: "db", comment: "channel number within the chip" },
            { name: "port", size: "db", comment: "output port for multiples of SN chip" },

            { name: "freq", size: "dw", comment: "current fnum/tone of the voice" },
            { name: "target_freq", size: "dw", comment: "target fnum/tone used for portamento" },

            { name: "volume", size: "db" },
            { name: "midi_note", size: "db" },
            { name: "instrument_num", size: "db" },
            
            { name: "slide_amount", size: "db", comment: "how much to add/subtract per tic" },
            { name: "slide_type", size: "db", comment: "type of slide (up/down/portamento)" },

            { name: "arpeggio_pos", size: "db"},
            { name: "arpeggio", size: 2 },

            { name: "order_table_ptr", size: "dw", comment: "pointer to the current order"},
            { name: "pattern_ptr", size: "dw", comment: "pointer to the current pattern" },
            { name: "line_wait", size: "db", comment: "wait for this many lines"},
            
            { name: "volume_macro_len", size: "db"},
            { name: "volume_macro_pos", size: "db"},
            
            { name: "volume_macro_mode", size: "db"},
            { name: "volume_macro_loop", size: "db"},
            
            { name: "volume_macro_ptr", size: "dw"},
            
            { name: "fm_patch_shifted", size: "db"},
            
            { name: "fm_base_reg", size: "db" },
            { name: "fm_drum_trigger", size: "db" },
            { name: "fm_drum_volume_mask", size: "db" },
        ]
    },
    {
        name: "music_state",
        members: [
            { name: "bank", size: "db", },
            { name: "channel_count", size: "db", },

            { name: "loop", size: "db", },
	
            { name: "sfx_channel", size: "db", },
                
            { name: "has_sn", size: "db", },
            { name: "has_fm", size: "db", },
            { name: "has_fm_drums", size: "db", },
                
            { name: "time_base", size: "db", },
            { name: "speed_1", size: "db", },
            { name: "speed_2", size: "db", },

            { name: "pattern_length", size: "db", },
            { name: "orders_length", size: "db", },
                
            { name: "instrument_ptrs", size: "dw", },
            { name: "order_ptrs", size: "dw", },
                
            { name: "channel_types", size: 32, },

            { name: "subtic", size: "db", },
            { name: "tic", size: "db", },
            { name: "line", size: "db", },
            { name: "order", size: "db", },

            { name: "process_new_line", size: "db", },
            
            { name: "noise_mode", size: "db", },
        ]
    },
];

// output asm file
let out_wladx = fs.openSync("music_driver/defines_wladx.inc", "w+");
let out_sdas = fs.openSync("music_driver_sdas/defines_sdas.inc", "w+");


function writelabel(label)
{
    fs.writeSync(outfile, song_prefix + "_" + label + ":" + EOL);
}

// write out defines
for (let i = 0; i < defines.length; i++)
{
    for (let j = 0; j < defines[i].length; j++)
    {
        let define = defines[i][j];

        // wladx defines
        fs.writeSync(out_wladx, ".define " + define.name + "\t\t" + "0x" + define.value.toString(16) + "\n");

        // sdas defines
        fs.writeSync(out_sdas, define.name + "\t\t\t.equ\t\t" + "0x" + define.value.toString(16) + "\n");
    }

    fs.writeSync(out_wladx, "\n");
    fs.writeSync(out_sdas, "\n");
}

// write out structs
for (let i = 0; i < structs.length; i++)
{
    let struct = structs[i];
    let byte_offset = 0;

    // start wla dx struct
    fs.writeSync(out_wladx, ".STRUCT " + struct.name + "\n");    

    for (let j = 0; j < struct.members.length; j++)
    {
        let member = struct.members[j];
        let comment = member.hasOwnProperty("comment") ? ("\t\t; " + member.comment) : "";

        if (member.size == "db")
        {
            fs.writeSync(out_wladx, "\t" + member.name + ": " + member.size + comment + "\n");
            fs.writeSync(out_sdas, struct.name + "." + member.name + " .equ " + byte_offset + comment + "\n");

            byte_offset += 1;
        }
        else if (member.size == "dw")
        {
            fs.writeSync(out_wladx, "\t" + member.name + ": " + member.size + comment + "\n");
            fs.writeSync(out_sdas, struct.name + "." + member.name + " .equ " + byte_offset + comment + "\n");

            byte_offset += 2;
        }
        else
        {
            fs.writeSync(out_wladx, "\t" + member.name + ": ds " + member.size + comment + "\n");
            fs.writeSync(out_sdas, struct.name + "." + member.name + " .equ " + byte_offset + comment + "\n");

            byte_offset += member.size;
        }
        
    }

    // end wla dx struct
    fs.writeSync(out_wladx, ".ENDST\n");

    fs.writeSync(out_sdas, "_sizeof_" + struct.name + " .equ " + byte_offset + "\n");

    fs.writeSync(out_wladx, "\n");
    fs.writeSync(out_sdas, "\n");
}

// close files
fs.closeSync(out_wladx);
fs.closeSync(out_sdas);