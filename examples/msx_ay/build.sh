python3 ../../furnace2json.py -o ./cmajor_ay.json ../cmajor_ay.fur
python3 ../../json2sms.py -o cmajor_ay.asm -i cmajor_ay ./cmajor_ay.json

python3 ../../furnace2json.py -o ./cmajor.json ../cmajor.fur
python3 ../../json2sms.py -o cmajor.asm -i cmajor ./cmajor.json 

wla-z80 -D BANJO_SYS=2 main.asm
wlalink -s -r linkfile.txt main.rom