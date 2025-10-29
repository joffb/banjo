# Joe Kennedy 2024
# Create asm include of SN76489 fnums

import math

note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]

MUSIC_CLOCK = 3579545.0
STEP_COUNT = 16

# start at the lowest unique & accurate pitch
# midi A2/furnace A0
START_NOTE = 33

# midi note frequencies for lowest octave of 12 notes
# with a number of steps in between
midi_note_frequencies = []

for n in range(0, 12):
    midi_note_frequencies.append([])
    for s in range(0, STEP_COUNT):
        midi_note_frequencies[n].append(math.pow(2.0, (n + (s/STEP_COUNT) + START_NOTE - 69)/12.0) * 440.0)

# fnum values for the sn chip for the given frequencies
fnums = [];

for n in range(0, 12):
    fnums.append([])
    for s in range(0, STEP_COUNT):
        freq = round(MUSIC_CLOCK / ( 2 * 8 * midi_note_frequencies[n][s]))
        freq = 4095 if freq > 4095 else freq
        fnums[n].append(freq)

outfile = open("music_driver/ay/fnums_ay.inc", "w")

outfile.write("ay_tone_lookup:\n")

for n in range(0, 12):
    note = (START_NOTE + n)
    outfile.write("; " + note_names[note % 12] + "\n")
    outfile.write(".dw " + ", ".join(map(hex, fnums[n])) + "\n")

outfile.close()