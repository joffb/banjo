
python3 ../../furnace2json.py -o ./cmajor.json ../cmajor.fur
python3 ../../json2sms.py -o cmajor.asm -i cmajor --sdas -a LIT_ -b 255 ./cmajor.json 
lcc -mz80:sms -c -autobank -o cmajor.o cmajor.asm

python3 ../../furnace2json.py -o ./cmajor_sn.json ../cmajor_sn.fur
python3 ../../json2sms.py -o cmajor_sn.asm -i cmajor_sn --sdas -a LIT_ -b 255 ./cmajor_sn.json
lcc -mz80:sms -c -autobank -o cmajor_sn.o cmajor_sn.asm

python3 ../../furnace2json.py -o ./sfx_test_sn.json ../sfx_test_sn.fur
python3 ../../json2sms.py -s 1 -o sfx_test_sn.asm -i sfx_test_sn --sdas -a LIT_ -b 255 ./sfx_test_sn.json
lcc -mz80:sms -c -autobank -o sfx_test_sn.o sfx_test_sn.asm

python3 ../../furnace2json.py -o ./sfx_test.json ../sfx_test.fur
python3 ../../json2sms.py -s 2 -o sfx_test.asm -i sfx_test --sdas -a LIT_ -b 255 ./sfx_test.json
lcc -mz80:sms -c -autobank -o sfx_test.o sfx_test.asm

lcc -mz80:sms -c -autobank -o main.o -I../../music_driver_sdas main.c

lcc -o gbdktest.sms -mz80:sms -Wl-m -autobank \
    main.o \
    ../../lib/gbdk_sms/banjo.rel \
    ../../lib/gbdk_sms/banjo_sn.rel \
    ../../lib/gbdk_sms/banjo_opll.rel \
    ../../lib/gbdk_sms/banjo_opll_drums.rel \
    ../../lib/banjo_sfx.rel \
    cmajor.o cmajor_sn.o sfx_test_sn.o sfx_test.o