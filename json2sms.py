
# json2sms
# Joe Kennedy 2024

import sys
import json
import math

MAGIC_BYTE          = 0xba

NOTE_ON 			= 0x00
NOTE_OFF 			= 0x01
INSTRUMENT_CHANGE	= 0x02
FM_PATCH 			= 0x03
FM_DRUM 			= 0x04
SN_NOISE_MODE		= 0x05
PITCH_SLIDE_UP	    = 0x06
PITCH_SLIDE_DOWN	= 0x07
ARPEGGIO			= 0x08
PORTAMENTO		    = 0x09
LINE_WAIT			= 0x0a
END_PATTERN_LINE	= 0xff

INSTRUMENT_SIZE	= 16
FM_PATCH_SIZE = 8

CHIP_SN76489 = 0x03		    # 0x03: SMS (SN76489) - 4 channels
CHIP_SN76489_OPLL = 0x43	# 0x43: SMS (SN76489) + OPLL (YM2413) - 13 channels (compound!)
CHIP_OPLL = 0x89			# 0x89: OPLL (YM2413) - 9 channels
CHIP_OPLL_DRUMS = 0xa7	    # 0xa7: OPLL drums (YM2413) - 11 channels

CHAN_SN76489 = 0x00
CHAN_OPLL = 0x01
CHAN_OPLL_DRUMS = 0x02

# whether the drum channel needs its volume to be pre-shifted
pre_shift_fm_drum_value = [0,0,4,0,4]

if (sys.argv[1] == "sfx"):
    sfx = True
    sfx_channel = int(sys.argv[2])
    song_prefix = sys.argv[3]
    in_filename = sys.argv[4]
    out_filename = sys.argv[5]
else:
    sfx = False
    sfx_channel = 0
    song_prefix = sys.argv[1]
    in_filename = sys.argv[2]
    out_filename = sys.argv[3]

# try to load json file
try:
    infile = open(in_filename, "r")
except OSError:
    print("Error reading input file: " + in_filename)
    sys.exit()

# read into variable
json_text = infile.read()
infile.close()

# try to parse json
try:
    song = json.loads(json_text)
except ValueError:
    print("File is not a valid json file: " + in_filename)
    sys.exit()


# flags to say which chips are active
song['has_sn'] = False
song['has_fm'] = False
song['has_fm_drums'] = False

if (CHIP_SN76489 in song['sound_chips']):
    song['has_sn'] = True

if (CHIP_OPLL in song['sound_chips']):
    song['has_fm'] = True

if (CHIP_OPLL_DRUMS in song['sound_chips']):
    song['has_fm_drums'] = True

# channel types and channel numbers
sn_count = 0
channel_types = []

for sound_chip in song['sound_chips']:

    if (sound_chip == CHIP_SN76489):
    
        for i in range(0, 4):
        
            channel_types.append(CHAN_SN76489)
            channel_types.append(i | (sn_count << 4))
        
        sn_count += 1
    

    elif (sound_chip == CHIP_OPLL):

        for i in range(0, 9):

            channel_types.append(CHAN_OPLL)
            channel_types.append(i)

    elif (sound_chip == CHIP_OPLL_DRUMS):

        for i in range(0, 6):

            channel_types.append(CHAN_OPLL)
            channel_types.append(i)
        
        for i in range(0, 5):
        
            channel_types.append(CHAN_OPLL_DRUMS)
            channel_types.append(i)
        
    elif (sound_chip == CHIP_SN76489_OPLL):
    
        for i in range(0, 4):
        
            channel_types.append(CHAN_SN76489)
            channel_types.append(i | (sn_count << 4))
        
        for i in range(0, 9):
        
            channel_types.append(CHAN_OPLL)
            channel_types.append(i)
        

        sn_count += 1

# in sfx mode, only include channel_types for the specified sfx channel
if (sfx):

    new_channel_types = [0, 0]
    
    new_channel_types[0] = channel_types[(sfx_channel * 2)]
    new_channel_types[1] = channel_types[(sfx_channel * 2) + 1]
    
    channel_types = new_channel_types


# fill out channel types array
for i in range (0, 32 - len(channel_types)):

    channel_types.append(0xff)

# process instruments

instrument_offsets = [0x0000]
instruments = []

volume_macros = []

for i in range(0, len(song['instruments'])):

    instrument = song['instruments'][i]
    instrument_bin = []

    # organise volume macro data
    if ("volume_macro" in instrument):
    
        instrument_bin.append(instrument['volume_macro']['length'])
        instrument_bin.append(instrument['volume_macro']['mode'])

        # if loop == 0xff, there's no looping
        if (instrument['volume_macro']['loop'] == 0xff):
            instrument_bin.append(instrument['volume_macro']['loop'])

        # loop position is > loop length is invalid
        elif (instrument['volume_macro']['loop'] > instrument['volume_macro']['length']):
            instrument_bin.append(0xff)

        # otherwise we do: loop = loop length - loop point
        # so when the loop ends we can do
        #   loop pointer -= loop value
        #   loop position -= loop value
        # to go to the start of the loop
        else:
            instrument_bin.append(instrument['volume_macro']['length'] - instrument['volume_macro']['loop'])
        
        # index of the volume macro
        instrument_bin.append(len(volume_macros))
        
        # invert volume data so 0 is loud and 15 is silent
        for j in range(0, len(instrument['volume_macro']['data'])):
        
            instrument['volume_macro']['data'][j] = 15 - instrument['volume_macro']['data'][j]
        
        
        # add macro data to separate arrays and set up offset of next volume macro (if there is one)
        volume_macros.append(instrument['volume_macro']['data'])
    
    # no volume_macro
    else:
    
        instrument_bin.append(0)
        instrument_bin.append(0)
        instrument_bin.append(0)
        instrument_bin.append(0xffff)
    
    
    # FM preset change (only for OPLL FM instruments)
    if (instrument['type'] == 13 and "fm" in instrument):
    
        # custom fm patch data
        if (instrument['fm']['opll_patch'] == 0):
        
            # pre-shift the patch number into the upper nibble
            instrument_bin.append((instrument['fm']['opll_patch'] & 0xf) << 4)

            fm_patch = [0, 0, 0, 0, 0, 0, 0, 0]
            operator = False

            for j in range(0, 2):
            
                operator = instrument['fm']['operator_data'][j]
                
                # operator sustain seems to be in top bit of ssg in furnace format
                fm_patch[0 + j] =   operator['mult'] | \
                                    (operator['ksr'] << 4) | \
                                    (((operator['ssg'] >> 3) & 0x1) << 5) | \
                                    (operator['vib'] << 6) | \
                                    (operator['am'] << 7)
            
            
            operator = instrument['fm']['operator_data'][0]
            fm_patch[2] = operator['tl'] | (operator['ksl'] << 6)
            
            # op 1 and 2 half-sine modes seem to be in instrument's fms and ams in furnace format
            operator = instrument['fm']['operator_data'][1]
            fm_patch[3] =   (instrument['fm']['feedback']) | \
                            (instrument['fm']['ams'] << 3) | \
                            (instrument['fm']['fms'] << 4) | \
                            (operator['ksl'] << 6)
            
            for j in range(0, 2):
            
                operator = instrument['fm']['operator_data'][j]
                fm_patch[4 + j] = operator['dr'] | (operator['ar'] << 4)
                fm_patch[6 + j] = operator['rr'] | (operator['sl'] << 4)
            
            
            for j in range(0, 8):
            
                instrument_bin.append(fm_patch[j])
            

        # custom drum patch with fixed pitch
        elif (instrument['fm']['opll_patch'] == 16):
        
            # pre-shift the patch number into the upper nibble
            instrument_bin.append(0xff if instrument['opl_drums']['fixed_freq'] else 0)

            instrument_bin.append(instrument['opl_drums']['kick_freq'] & 0xff)
            instrument_bin.append(instrument['opl_drums']['kick_freq'] >> 8)
            
            instrument_bin.append(instrument['opl_drums']['snare_hat_freq'] & 0xff)
            instrument_bin.append(instrument['opl_drums']['snare_hat_freq'] >> 8)

            instrument_bin.append(instrument['opl_drums']['tom_top_freq'] & 0xff)
            instrument_bin.append(instrument['opl_drums']['tom_top_freq'] >> 8)

            instrument_bin.append(0xff)
            instrument_bin.append(0xff)
        
        
        # using a standard preset so no extra fm data
        else:
        
            # pre-shift the patch number into the upper nibble
            instrument_bin.append((instrument['fm']['opll_patch'] & 0xf) << 4)

            for j in range(0, 8):
            
                instrument_bin.append(0xff)
        
    # not an FM instrument
    else:

        instrument_bin.append(0xff)
        
        for j in range(0, 8):

            instrument_bin.append(0xff)
        
    instruments.append(instrument_bin)
    instrument_offsets.append(instrument_offsets[i] + INSTRUMENT_SIZE)

# process patterns

patterns = {}

# process patterns
# go through each channel
for i in range(0, song['channel_count']):

    # in sfx mode, only process sfx channel
    if (sfx and i != sfx_channel):
    
        continue
    
    channel_type = channel_types[i * 2]
    channel_subchannel = channel_types[(i * 2) + 1]
    
    # only one channel for sfx
    if (sfx):
    
        channel_type = channel_types[0]
        channel_subchannel = channel_types[1]
    
    
    # separate pattern arrays per channel
    patterns[i] = []

    current_inst = -1
    current_vol = 15

    # go through each pattern in the channel
    for j in range (0, len(song['patterns'][i])):
    
        pattern = song['patterns'][i][j]
        
        pattern_bin = []
        
        if (pattern):
        
            # go through every line
            for k in range (0, song['pattern_length']):
            
                line = pattern['data'][k]
                
                # instrument has changed
                if (current_inst != line['instrument'] and line['instrument'] != -1):
                
                    current_inst = line['instrument']

                    if (channel_type == CHAN_OPLL_DRUMS):
                    
                        pattern_bin.append("FM_DRUM")
                        pattern_bin.append(current_inst)
                    
                    else:
                    
                        pattern_bin.append("INSTRUMENT_CHANGE")
                        pattern_bin.append(current_inst)
                    
                # check effects
                for eff in range (0, len(line['effects']), 2):
                
                    # SN noise mode
                    if (line['effects'][eff] == 0x20):
                    
                        pattern_bin.append("SN_NOISE_MODE")
                        
                        noise_mode = (line['effects'][eff + 1] & 0x1) << 2
                        noise_freq = 0x3 if (line['effects'][eff + 1] & 0x10) else 0x0

                        pattern_bin.append(0x80 | (0x3 << 5) | noise_mode | noise_freq)
                    
                    
                    # Arpeggio
                    elif (line['effects'][eff] == 0x00):
                    
                        pattern_bin.append("ARPEGGIO")
                        pattern_bin.append(line['effects'][eff + 1] >> 4)
                        pattern_bin.append(line['effects'][eff + 1] & 0xf)
                    

                    # Pitch slide up
                    elif (line['effects'][eff] == 0x01):
                    
                        pattern_bin.append("PITCH_SLIDE_UP")
                        pattern_bin.append(line['effects'][eff + 1])
                    
                    
                    # Pitch slide down
                    elif (line['effects'][eff] == 0x02):
                    
                        pattern_bin.append("PITCH_SLIDE_DOWN")
                        pattern_bin.append(line['effects'][eff + 1])
                    

                    # Portamento
                    elif (line['effects'][eff] == 0x03):
                    
                        pattern_bin.append("PORTAMENTO")
                        pattern_bin.append(line['effects'][eff + 1])
                
                # empty
                if (line['note'] == 0 and line['octave'] == 0):
                
                    False
                    
                # note on
                elif (line['note'] <= 12):
                
                    pattern_bin.append("NOTE_ON")
                    
                    volume = current_vol if (line['volume'] == -1) else line['volume']
                    
                    # sn channel, this byte can be sent straight to the output to set the volume
                    if (channel_type == CHAN_SN76489):
                    
                        pattern_bin.append((15 - volume) | 0x80 | 0x10 | ((channel_subchannel & 0xf) << 5))
                    
                    elif (channel_type == CHAN_OPLL):
                    
                        pattern_bin.append((15 - volume))
                    
                    elif (channel_type == CHAN_OPLL_DRUMS):
                    
                        pattern_bin.append((15 - volume) << pre_shift_fm_drum_value[channel_subchannel])
                    
                
                    # note number
                    if (channel_type == CHAN_SN76489):
                    
                        pattern_bin.append((line['note'] + (line['octave'] * 12) + 36 - 45) & 0x7f)
                    
                    else:
                    
                        pattern_bin.append((line['note'] + (line['octave'] * 12)) & 0x7f)
                    
                
                    if (line['volume'] != -1):
                    
                        current_vol = line['volume']
                    
                
                # note off
                elif (line['note'] == 100):
                
                    pattern_bin.append("NOTE_OFF")
                
                
                # end line marker
                pattern_bin.append(END_PATTERN_LINE)
            

            # when a run of END_PATTERN_LINE ends, replace it if it's long enough
            def run_end(run_length, pattern_bin_rle):
            
                if (run_length > 2):
                
                    pattern_bin_rle.append("LINE_WAIT")
                    pattern_bin_rle.append(run_length - 1)
                
                elif (run_length > 0):
                
                    for k in range(0, run_length):
                    
                        pattern_bin_rle.append(END_PATTERN_LINE)
                    

            # run length encoding for END_PATTERN_LINEs
            run_length = 0
            pattern_bin_rle = []
            

            # look through pattern_bin for runs of END_PATTERN_LINE
            for k in range(0, len(pattern_bin)):
            
                if (pattern_bin[k] == END_PATTERN_LINE):

                    run_length += 1
                
                else:
                
                    run_end(run_length, pattern_bin_rle)
                    run_length = 0
                    pattern_bin_rle.append(pattern_bin[k])
                
            run_end(run_length, pattern_bin_rle)
            
            pattern_bin = pattern_bin_rle

            # add to patterns array
            patterns[i].append(pattern_bin)        

# output asm/h file
outfile = open(out_filename, "w")

# asm format output
if (out_filename.endswith("asm")):

    def writelabel(label):

        outfile.write(song_prefix + "_" + label + ":" + "\n")


    # initial label
    outfile.write(song_prefix + ":" + "\n")

    # basic song data
    writelabel("magic_byte")
    outfile.write(".db " + str(MAGIC_BYTE) + "\n")
    writelabel("bank")
    outfile.write(".db 0\n")
    writelabel("channel_count")
    outfile.write(".db " + str(1 if sfx else song['channel_count']) + "\n")
    writelabel("loop")
    outfile.write(".db " + str(0 if sfx else 1) + "\n")
    writelabel("sfx_channel")
    outfile.write(".db " + str(sfx_channel if sfx else 0xff) + "\n")
    writelabel("has_sn")
    outfile.write(".db " + str(1 if song['has_sn'] else 0) + "\n")
    writelabel("has_fm")
    outfile.write(".db " + str(1 if song['has_fm'] else 0) + "\n")
    writelabel("has_fm_drums")
    outfile.write(".db " + str(1 if song['has_fm_drums'] else 0) + "\n")
    writelabel("time_base")
    outfile.write(".db " + str(song['time_base'] + 1) + "\n")
    writelabel("speed_1")
    outfile.write(".db " + str(song['speed_1']) + "\n")
    writelabel("speed_2")
    outfile.write(".db " + str(song['speed_2']) + "\n")
    writelabel("pattern_length")
    outfile.write(".db " + str(song['pattern_length']) + "\n")
    writelabel("orders_length")
    outfile.write(".db " + str(song['orders_length']) + "\n")
    writelabel("instrument_ptrs")
    outfile.write(".dw " + song_prefix + "_instrument_pointers" + "\n")
    writelabel("order_ptrs")
    outfile.write(".dw " + song_prefix + "_order_pointers" + "\n")
    writelabel("channel_types")
    outfile.write(".db " + ", ".join(map(str, channel_types)) + "\n")

    outfile.write("\n" + "\n")

    # volume macros
    writelabel("volume_macros")

    for i in range (0, len(volume_macros)):

        writelabel("volume_macro_" + str(i))
        
        outfile.write(".db " + ", ".join(map(str, volume_macros[i])) + "\n")


    # instruments
    writelabel("instrument_pointers")

    for i in range (0, len(instruments)):

        outfile.write(".dw " + song_prefix + "_instrument_" + str(i) + "\n")


    outfile.write("\n" + "\n")

    # instruments
    writelabel("instrument_data")

    for i in range (0, len(instruments)):

        instrument = instruments[i]
        
        writelabel("instrument_" + str(i))
        outfile.write("\t.db " + str(instrument[0]) + ", " + str(instrument[1]) + ", " + str(instrument[2]) + "\n")
        
        # volume macro pointer
        if (instrument[3] != 0xffff):
        
            outfile.write("\t.dw " + song_prefix + "_volume_macro_" + str(instrument[3]) + "\n")
        
        else:
        
            outfile.write("\t.dw 0xffff" + "\n")
        
        
        # fm preset
        outfile.write("\t.db " + str(instrument[4]) + "\n")
        
        # fm patch
        outfile.write("\t.db " + ", ".join(map(str, instrument[5:14])) + "\n")	


    outfile.write("\n" + "\n")


    # order pointers
    writelabel("order_pointers")

    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channel
        if (sfx and i != sfx_channel):
        
            continue
        
        
        outfile.write("\t.dw " + song_prefix + "_orders_channel_" + str(i) + "\n")


    # orders
    writelabel("orders")

    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channel
        if (sfx and i != sfx_channel):
        
            continue
        
        
        writelabel("orders_channel_" + str(i))		
        
        for j in range (0, len(song['orders'][i])):
            
            if (j % 4 == 0):
            
                outfile.write("\n" + "\t.dw ")
            
                
            outfile.write(song_prefix + "_patterns_" + str(i) + "_" + str(song['orders'][i][j]))

            if ((j % 4 != 3) and (j != len(song['orders'][i]) - 1)):
            
                outfile.write(", ")

        
        outfile.write("\n")


    outfile.write("\n" + "\n")

    # patterns
    writelabel("patterns")

    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channelfs.
        if (sfx and i != sfx_channel):
        
            continue
        
        for j in range (0, len(patterns[i])):
            writelabel("patterns_" + str(i) + "_" + str(j))
            outfile.write(".db " + ", ".join(map(str, patterns[i][j])) + "\n")

# c format output
elif (out_filename.endswith("c")):

    outfile.write('#include "banjo.h"\n\n')

    outfile.write("song_data_t const " + song_prefix + " = {" + "\n")

    outfile.write("\t" + str(MAGIC_BYTE) + ",\n")
    outfile.write("\t" + "0,\n")
    outfile.write("\t" + str(1 if sfx else song['channel_count']) + ",\n")
    outfile.write("\t" + str(0 if sfx else 1) + ",\n")
    outfile.write("\t" + str(sfx_channel if sfx else 0xff) + ",\n")
    outfile.write("\t" + str(1 if song['has_sn'] else 0) + ",\n")
    outfile.write("\t" + str(1 if song['has_fm'] else 0) + ",\n")
    outfile.write("\t" + str(1 if song['has_fm_drums'] else 0) + ",\n")
    outfile.write("\t" + str(song['time_base'] + 1) + ",\n")
    outfile.write("\t" + str(song['speed_1']) + ",\n")
    outfile.write("\t" + str(song['speed_2']) + ",\n")
    outfile.write("\t" + str(song['pattern_length']) + ",\n")
    outfile.write("\t" + str(song['orders_length']) + ",\n")
    outfile.write("\t" + song_prefix + "_instrument_pointers" + ",\n")
    outfile.write("\t" + song_prefix + "_order_pointers" + ",\n")
    outfile.write("\t{" + ", ".join(map(str, channel_types)) + " }\n")
    outfile.write("};\n\n")


    # volume macros
    for i in range (0, len(volume_macros)):

        outfile.write("unsigned char const " + song_prefix  + "_volume_macro_" + str(i) + "[] = {")
        outfile.write(", ".join(map(str, volume_macros[i])))
        outfile.write("};\n")

    outfile.write("\n")


    # instrument pointers
    outfile.write("instrument_t const * const " + song_prefix + "_instrument_pointers[] = {\n")

    for i in range (0, len(instruments)):

        outfile.write("\t&" + song_prefix + "_instrument_" + str(i) + ",\n")

    outfile.write("};\n\n")


    # instruments
    for i in range (0, len(instruments)):

        instrument = instruments[i]
        
        outfile.write("instrument_t const " + song_prefix + "_instrument_" + str(i) + " = {\n")
        outfile.write("\t" + str(instrument[0]) + ", " + str(instrument[1]) + ", " + str(instrument[2]) + ",\n")
        
        # volume macro pointer
        if (instrument[3] != 0xffff):
        
            outfile.write("\t" + song_prefix + "_volume_macro_" + str(instrument[3]) + ",\n")
        
        else:
        
            outfile.write("\t0xffff" + ",\n")
        
        
        # fm preset
        outfile.write("\t" + str(instrument[4]) + ",\n")
        
        # fm patch
        outfile.write("\t{" + ", ".join(map(str, instrument[5:14])) + "}\n")	

        outfile.write("};\n\n")


    # order pointers
    outfile.write("unsigned char const * const * const " + song_prefix + "_order_pointers[] = {\n")

    order_pointer_list = []

    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channel
        if (sfx and i != sfx_channel):
        
            continue

        outfile.write("\t" + song_prefix + "_orders_channel_" + str(i) + ",\n")

    outfile.write("};\n\n")


    # orders
    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channel
        if (sfx and i != sfx_channel):
        
            continue

        outfile.write("unsigned char const * const " + song_prefix + "_orders_channel_" + str(i) + "[] = {\n")

        for j in range (0, len(song['orders'][i])):
            
            outfile.write("\t" + song_prefix + "_patterns_" + str(i) + "_" + str(song['orders'][i][j]) + ",\n")

        outfile.write("};\n")

    outfile.write("\n\n")


    # patterns
    for i in range (0, song['channel_count']):

        # in sfx mode, only process sfx channelfs.
        if (sfx and i != sfx_channel):
        
            continue

        for j in range (0, len(patterns[i])):

            outfile.write("unsigned char const " + song_prefix + "_patterns_" + str(i) + "_" + str(j) + "[] = {\n")
            outfile.write("\t" + ", ".join(map(str, patterns[i][j])) + "\n")
            outfile.write("};\n")

    outfile.write("\n\n")

# close file
outfile.close()