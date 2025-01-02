
python ..\..\furnace2json.py -o .\cmajor.json ..\cmajor.fur
python ..\..\json2sms.py -o cmajor.asm -i cmajor --sdas -a BANK -b 2 .\cmajor.json 
sdasz80 -g -o cmajor.rel ..\..\music_driver_sdas\banjo_defines_sdas.inc cmajor.asm

python ..\..\furnace2json.py -o .\cmajor_sn.json ..\cmajor_sn.fur
python ..\..\json2sms.py -o cmajor_sn.asm -i cmajor_sn --sdas -a BANK -b 3 .\cmajor_sn.json
sdasz80 -g -o cmajor_sn.rel ..\..\music_driver_sdas\banjo_defines_sdas.inc cmajor_sn.asm

python ..\..\furnace2json.py -o .\sfx_test.json ..\sfx_test.fur
python ..\..\json2sms.py -o sfx_test.asm -s 2 -i sfx_test --sdas -a BANK -b 2 .\sfx_test.json
sdasz80 -g -o sfx_test.rel ..\..\music_driver_sdas\banjo_defines_sdas.inc sfx_test.asm

python ..\..\furnace2json.py -o .\sfx_test_sn.json ..\sfx_test_sn.fur
python ..\..\json2sms.py -o sfx_test_sn.asm -s 2 -i sfx_test_sn --sdas -a BANK -b 3 .\sfx_test_sn.json
sdasz80 -g -o sfx_test_sn.rel ..\..\music_driver_sdas\banjo_defines_sdas.inc sfx_test_sn.asm


sdcc -c -I..\..\music_driver_sdas -mz80 main.c
sdcc -c -I..\..\music_driver_sdas -mz80 --codeseg BANK1 bank1.c
sdcc -o dktest.ihx -mz80 --no-std-crt0 --data-loc 0xC000 ^
    -Wl-b_BANK1=0x18000 -Wl-b_BANK2=0x28000 -Wl-b_BANK3=0x38000 ^
    crt0_sms.rel SMSlib.lib ^
    ..\..\music_driver_sdas\lib\sms\banjo.rel ^
    ..\..\music_driver_sdas\lib\sms\banjo.rel ^
    ..\..\music_driver_sdas\lib\sms\banjo_sn.rel ^
    ..\..\music_driver_sdas\lib\sms\banjo_opll.rel ^
    ..\..\music_driver_sdas\lib\sms\banjo_opll_drums.rel ^
    ..\..\music_driver_sdas\lib\banjo_sfx.rel ^
    ..\..\music_driver_sdas\lib\sms\banjo_queue.rel ^
    main.rel bank1.rel sfx_test.rel sfx_test_sn.rel cmajor.rel cmajor_sn.rel
makesms dktest.ihx dktest.sms