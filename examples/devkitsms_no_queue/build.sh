
python3 ../../furnace2json.py -o ./cmajor.json ../cmajor_opll_drum.fur
python3 ../../json2sms.py -o cmajor.asm -i cmajor --sdas -a BANK -b 2 ./cmajor.json 
sdasz80 -g -o cmajor.rel ../../music_driver_sdas/banjo_defines_sdas.inc cmajor.asm

python3 ../../furnace2json.py -o ./cmajor_sn.json ../cmajor_sn.fur
python3 ../../json2sms.py -o cmajor_sn.asm -i cmajor_sn --sdas -a BANK -b 3 ./cmajor_sn.json
sdasz80 -g -o cmajor_sn.rel ../../music_driver_sdas/banjo_defines_sdas.inc cmajor_sn.asm

python3 ../../furnace2json.py -o ./sfx_test.json ../sfx_test.fur
python3 ../../json2sms.py -o sfx_test.asm -s 2 -i sfx_test --sdas -a BANK -b 2 ./sfx_test.json
sdasz80 -g -o sfx_test.rel ../../music_driver_sdas/banjo_defines_sdas.inc sfx_test.asm

python3 ../../furnace2json.py -o ./sfx_test_sn.json ../sfx_test_sn.fur
python3 ../../json2sms.py -o sfx_test_sn.asm -s 1 -i sfx_test_sn --sdas -a BANK -b 3 ./sfx_test_sn.json
sdasz80 -g -o sfx_test_sn.rel ../../music_driver_sdas/banjo_defines_sdas.inc sfx_test_sn.asm


sdcc -c -I../../music_driver_sdas -mz80 main.c
sdcc -c -I../../music_driver_sdas -mz80 --codeseg BANK1 bank1.c
sdcc -o dktest.ihx -mz80 --no-std-crt0 --data-loc 0xC000 \
    -Wl-b_BANK1=0x18000 -Wl-b_BANK2=0x28000 -Wl-b_BANK3=0x38000 \
    crt0_sms.rel SMSlib.lib \
    ../../lib/devkitsms/banjo.rel \
    ../../lib/devkitsms/banjo_sn.rel \
    ../../lib/devkitsms/banjo_opll.rel \
    ../../lib/devkitsms/banjo_opll_drums.rel \
    ../../lib/banjo_sfx.rel \
    ../../lib/banjo_queue.rel \
    main.rel bank1.rel sfx_test.rel sfx_test_sn.rel cmajor.rel cmajor_sn.rel
makesms dktest.ihx dktest.sms