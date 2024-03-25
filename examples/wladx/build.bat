python ..\..\furnace2json.py -o .\cmajor.json ..\cmajor.fur
python ..\..\json2sms.py -o cmajor.asm -i cmajor .\cmajor.json 

python ..\..\furnace2json.py -o .\cmajor_sn.json ..\cmajor_sn.fur
python ..\..\json2sms.py -o cmajor_sn.asm -i cmajor_sn .\cmajor_sn.json 

python ..\..\furnace2json.py -o .\sfx_test.json ..\sfx_test.fur
python ..\..\json2sms.py -o sfx_test.asm -s 2 -i sfx_test .\sfx_test.json 

python ..\..\furnace2json.py -o .\sfx_test_sn.json ..\sfx_test_sn.fur
python ..\..\json2sms.py -o sfx_test_sn.asm -s 2 -i sfx_test_sn .\sfx_test_sn.json 

wla-z80 -D BANJO_MODE=5 main.asm
wlalink -s -r linkfile.txt main.sms
wlalink -s -r linkfile.txt main.gg