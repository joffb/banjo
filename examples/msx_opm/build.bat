python ..\..\furnace2json.py -o .\cmajor_ay.json ..\cmajor_ay.fur
python ..\..\json2sms.py -o cmajor_ay.asm -i cmajor_ay .\cmajor_ay.json 

python ..\..\furnace2json.py -o .\cmajor_opm.json ..\cmajor_opm.fur
python ..\..\json2sms.py -o cmajor_opm.asm -i cmajor .\cmajor_opm.json 

wla-z80 -D BANJO_SYS=2 main.asm
wlalink -s -r linkfile.txt main.rom
