python ..\..\furnace2json.py -o .\cmajor_ay.json ..\cmajor_ay.fur
python ..\..\json2sms.py -o .\cmajor_ay.asm -i cmajor_ay .\cmajor_ay.json 

wla-z80 -D BANJO_MODE=2 -D QUEUES_OFF=1 main.asm
wlalink -s -r linkfile.txt msx_ay_reg_test.rom