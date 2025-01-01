
    # c format output
    elif (out_filename.suffix.lower() == ".c"):

        if (options.area):
            outfile.write('#pragma constseg {:s}{:s}\n\n'.format(options.area, "_{:d}".format(int(options.bank)) if (options.bank) else ""))
        elif (options.bank):
            outfile.write('#pragma bank {:d}\n\n'.format(int(options.bank)))

        outfile.write('#include "banjo.h"\n\n')

        for call in channel_init_calls:
            outfile.write("extern void {:s}(void);\n".format(call))

        for call in channel_update_calls:
            outfile.write("extern void {:s}(void);\n".format(call))

        for call in song_mute_calls:
            outfile.write("extern void {:s}(void);\n".format(call))

        outfile.write("\n")

        if (options.bank):
            outfile.write("const void __at(255) __bank_" + song_prefix + ";\n")

        outfile.write("const song_data_t {:s} = {{\n".format(song_prefix))
        outfile.write("\t.magic_byte = 0x{:02x},\n".format(MAGIC_BYTE))
        outfile.write("\t.bank = {:d},\n".format(int(options.bank) if (options.bank) else 0))
        outfile.write("\t.channel_count = {:d},\n".format(1 if sfx else song['channel_count']))
        outfile.write("\t.flags = {:d},\n".format(0 if sfx else 2))
        outfile.write("\t.master_volume = 0x80,\n")
        outfile.write("\t.has_chips = {:d},\n".format(1 if song['has_chips'] else 0))
        outfile.write("\t.sfx_channel = {:d},\n".format(sfx_channel if sfx else 0xff))
        outfile.write("\t.sfx_subchannel = {:d},\n".format(sfx_subchannel if sfx else 0xff))
        outfile.write("\t.time_base = {:d},\n".format(song['time_base'] + 1))
        outfile.write("\t.speed_1 = {:d},\n".format(song['speed_1']))
        outfile.write("\t.speed_2 = {:d},\n".format(song['speed_2']))
        outfile.write("\t.pattern_length = {:d},\n".format(song['pattern_length']))
        outfile.write("\t.orders_length = {:d},\n".format(song['orders_length']))
        outfile.write("\t.instrument_pointers = {:s}_instrument_pointers,\n".format(song_prefix))
        outfile.write("\t.order_pointers = {:s}_order_pointers,\n".format(song_prefix))
        outfile.write("\t.subtic = 0,\n") # subtic
        outfile.write("\t.tic = 0,\n") # tic
        outfile.write("\t.line = 0,\n") # line
        outfile.write("\t.order = 0,\n") # order
        outfile.write("\t.order_jump = 0xff,\n") # order
        outfile.write("\t.noise_mode = 0,\n") # noise_mode
        outfile.write("\t.channel_init_calls_count = {:d},\n".format(len(channel_init_calls)))
        outfile.write("\t.channel_update_calls_count = {:d},\n".format(len(channel_update_calls)))
        outfile.write("\t.song_mute_calls_count = {:d},\n".format(len(channel_update_calls)))
        outfile.write("\t.channel_init_calls = {:s}_channel_init_calls_data,\n".format(song_prefix))
        outfile.write("\t.channel_update_calls = {:s}_channel_update_calls_data,\n".format(song_prefix))
        outfile.write("\t.channel_mute_calls = {:s}_channel_mute_calls_data,\n".format(song_prefix))
        outfile.write("\t.song_mute_calls = {:s}_song_mute_calls_data,\n".format(song_prefix))
        outfile.write("\t.stop_call = {:s},\n".format("banjo_sfx_stop" if sfx else "banjo_song_stop"))
        outfile.write("};\n\n")

        # channel init calls
        outfile.write("static const banjo_func_ptr_t {:s}_channel_init_calls_data[] = {{\n".format(song_prefix))
        outfile.write("\t" + ", ".join(channel_init_calls) + "\n")
        outfile.write("};\n\n")

        # channel update calls
        outfile.write("static const banjo_func_ptr_t {:s}_channel_update_calls_data[] = {{\n".format(song_prefix))
        outfile.write("\t" + ", ".join(channel_update_calls) + "\n")
        outfile.write("};\n\n")

        # channel mute calls
        outfile.write("static const banjo_func_ptr_t {:s}_channel_mute_calls_data[] = {{\n".format(song_prefix))
        outfile.write("\t" + ", ".join(channel_mute_calls) + "\n")
        outfile.write("};\n\n")

        # song mute calls
        outfile.write("static const banjo_func_ptr_t {:s}_song_mute_calls_data[] = {{\n".format(song_prefix))
        outfile.write("\t" + ", ".join(song_mute_calls) + "\n")
        outfile.write("};\n\n")

        # macros
        for key in macros:
            outfile.write("static const unsigned char {:s}_{:s}[] = {{ {:s} }};\n".format(song_prefix, key, ", ".join(map(str, macros[key]))))

        outfile.write("\n")

        # fm patches
        for key in fm_patches:
            outfile.write("static const unsigned char {:s}_{:s}[] = {{ {:s} }};\n".format(song_prefix, key, ", ".join(map(str, fm_patches[key]))))

        outfile.write("\n")

        if len (instruments) > 0:

            # instrument pointers
            outfile.write("static const instrument_t * const {:s}_instrument_pointers[] = {{\n".format(song_prefix))

            for i in range (0, len(instruments)):

                outfile.write("\t&{:s}_instrument_{:d},\n".format(song_prefix, i))

            outfile.write("};\n\n")


        # instruments
        for i in range (0, len(instruments)):

            instrument = instruments[i]

            outfile.write("static const instrument_t {:s}_instrument_{:d} = {{\n".format(song_prefix, i))
            outfile.write("\t.volume_macro_len = 0x{:02x},\n".format(instrument["volume_macro_len"]))
            outfile.write("\t.volume_macro_loop = 0x{:02x},\n".format(instrument["volume_macro_loop"]))
            outfile.write("\t.volume_macro_ptr = {:s},\n".format(str(instrument["volume_macro_ptr"])))

            outfile.write("\t.ex_macro_type = {:s},\n".format(str(instrument["ex_macro_type"])))
            outfile.write("\t.ex_macro_len = 0x{:02x},\n".format(instrument["ex_macro_len"]))            
            outfile.write("\t.ex_macro_loop = 0x{:02x},\n".format(instrument["ex_macro_loop"]))
            outfile.write("\t.ex_macro_ptr = {:s},\n".format(str(instrument["ex_macro_ptr"])))

            # fm preset and patch
            outfile.write("\t.fm_preset = 0x{:02x},\n".format(instrument["fm_patch"]))
            outfile.write("\t.fm_patch = {:s}\n".format(str(instrument["fm_patch_ptr"])))

            outfile.write("};\n\n")


        # order pointers
        outfile.write("static const unsigned char * const * const {:s}_order_pointers[] = {{\n".format(song_prefix))

        order_pointer_list = []

        for i in range (0, song['channel_count']):

            # in sfx mode, only process sfx channel
            if (sfx and i != sfx_channel):

                continue

            outfile.write("\t{:s}_orders_channel_{:d},\n".format(song_prefix, i))

        outfile.write("};\n\n")


        # orders
        for i in range (0, song['channel_count']):

            # in sfx mode, only process sfx channel
            if (sfx and i != sfx_channel):

                continue

            outfile.write("static const unsigned char * const {:s}_orders_channel_{:d}[] = {{\n".format(song_prefix, i))

            for j in range (0, len(song['orders'][i])):

                outfile.write("\t{:s}_patterns_{:d}_{:d},\n".format(song_prefix, i, song['orders'][i][j]))

            outfile.write("};\n")

        outfile.write("\n\n")


        # patterns
        for i in range (0, song['channel_count']):

            # in sfx mode, only process sfx channelfs.
            if (sfx and i != sfx_channel):

                continue

            for j in range (0, len(song["patterns"][i])):

                pattern_index = song["patterns"][i][j]["index"]

                outfile.write("static const unsigned char {:s}_patterns_{:d}_{:d}[] = {{\n\t{:s}\n}};\n".format(song_prefix, i, pattern_index, ", ".join(map(str, patterns[i][pattern_index]))))

        outfile.write("\n\n")