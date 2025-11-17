# Joe Kennedy 2024
# Create asm include of fnums

import math

note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]

chips = [
    { "filename": "fnums_4mhz.inc", "clock": 3993600.0 },
    { "filename": "fnums_3p57mhz.inc", "clock": 3579545.0 },
]

for chip in chips:

    MUSIC_CLOCK = chip["clock"]
    MUSIC_FM_FSAM = (MUSIC_CLOCK / 72.0)
    STEP_COUNT = 16

    # start at the lowest unique & accurate pitch
    # where the octave fits into 12 bits
    START_NOTE = 12

    # midi note frequencies for lowest octave of 12 notes
    # with a number of steps in between
    midi_note_frequencies = []

    for n in range(0, 12):
        midi_note_frequencies.append([])
        for s in range(0, STEP_COUNT):
            midi_note_frequencies[n].append(math.pow(2.0, (n + (s/STEP_COUNT) + START_NOTE - 69)/12.0) * 440.0)

    # fnum values for the sn chip for the given frequencies
    fnums = []

    for n in range(0, 12):
        fnums.append([])
        for s in range(0, STEP_COUNT):
            fnum = round((144.0 * midi_note_frequencies[n][s] * math.pow(2.0, 20))/ MUSIC_CLOCK) 
            fnums[n].append(fnum)

    outfile = open("music_driver/opn/" + chip["filename"], "w")

    outfile.write("opn_tone_lookup:\n")

    for n in range(0, 12):
        note = (START_NOTE + n)
        outfile.write("; " + note_names[note % 12] + "\n")
        outfile.write(".dw " + ", ".join(map(hex, fnums[n])) + "\n")

    outfile.close()