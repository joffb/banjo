python ..\..\furnace2json.py ..\cmajor.fur cmajor.json
python ..\..\json2sms.py cmajor cmajor.json cmajor.asm

python ..\..\furnace2json.py ..\cmajor_sn.fur cmajor_sn.json
python ..\..\json2sms.py cmajor_sn cmajor_sn.json cmajor_sn.asm

python ..\..\furnace2json.py ..\sfx_test.fur sfx_test.json
python ..\..\json2sms.py sfx 2 sfx_test sfx_test.json sfx_test.asm

python ..\..\furnace2json.py ..\sfx_test_sn.fur sfx_test_sn.json
python ..\..\json2sms.py sfx 2 sfx_test_sn sfx_test_sn.json sfx_test_sn.asm

wla-z80 -D BANJO_MODE=2 -D QUEUES_OFF=1 main.asm
wlalink -s -r linkfile.txt main.sms
wlalink -s -r linkfile.txt main.gg