assets2banks assets

python ..\..\furnace2json.py ..\cmajor.fur cmajor.json
python ..\..\json2sms.py cmajor cmajor.json cmajor.c

python ..\..\furnace2json.py ..\cmajor_sn.fur cmajor_sn.json
python ..\..\json2sms.py cmajor_sn cmajor_sn.json cmajor_sn.c

python ..\..\furnace2json.py ..\sfx_test.fur sfx_test.json
python ..\..\json2sms.py sfx 2 sfx_test sfx_test.json sfx_test.c

python ..\..\furnace2json.py ..\sfx_test_sn.fur sfx_test_sn.json
python ..\..\json2sms.py sfx 2 sfx_test_sn sfx_test_sn.json sfx_test_sn.c

sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 -c main.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK1 -c bank1.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c bank2.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c cmajor.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK3 -c cmajor_sn.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c sfx_test.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK3 -c sfx_test_sn.c
sdcc -o visualiser.ihx -mz80 --no-std-crt0 --data-loc 0xC000 -Wl-b_BANK1=0x18000 -Wl-b_BANK2=0x28000 -Wl-b_BANK3=0x38000 crt0_sms.rel ..\..\music_driver_sdas\lib\music_driver_fm_drums.rel main.rel SMSlib.lib bank1.rel bank2.rel sfx_test.rel sfx_test_sn.rel cmajor.rel cmajor_sn.rel
makesms visualiser.ihx visualiser.sms