assets2banks assets

python ..\..\furnace2json.py -o .\cmajor.json ..\cmajor.fur
python ..\..\json2sms.py -o cmajor.c -i cmajor .\cmajor.json 

python ..\..\furnace2json.py -o .\cmajor_sn.json ..\cmajor_sn.fur
python ..\..\json2sms.py -o cmajor_sn.c -i cmajor_sn .\cmajor_sn.json 

python ..\..\furnace2json.py -o .\sfx_test.json ..\sfx_test.fur
python ..\..\json2sms.py -o sfx_test.c -s 2 -i sfx_test .\sfx_test.json 

python ..\..\furnace2json.py -o .\sfx_test_sn.json ..\sfx_test_sn.fur
python ..\..\json2sms.py -o sfx_test_sn.c -s 2 -i sfx_test_sn .\sfx_test_sn.json 

sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 -c main.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK1 -c bank1.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c bank2.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c cmajor.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK3 -c cmajor_sn.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK2 -c sfx_test.c
sdcc --peep-file peep-rules.txt -I..\..\music_driver_sdas -mz80 --codeseg BANK3 -c sfx_test_sn.c
sdcc -o visualiser.ihx -mz80 --no-std-crt0 --data-loc 0xC000 -Wl-b_BANK1=0x18000 -Wl-b_BANK2=0x28000 -Wl-b_BANK3=0x38000 crt0_sms.rel ..\..\music_driver_sdas\lib\music_driver_fm_drums.rel main.rel SMSlib.lib bank1.rel bank2.rel sfx_test.rel sfx_test_sn.rel cmajor.rel cmajor_sn.rel
makesms visualiser.ihx visualiser.sms