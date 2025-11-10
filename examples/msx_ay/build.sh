python3 ../../furnace2json.py -o ./cmajor_ay.json ../cmajor_ay.fur
python3 ../../json2sms.py -o cmajor_ay.asm -i cmajor_ay ./cmajor_ay.json

python ../../furnace2json.py -o ./cmajor_ay_opll.json ../cmajor_ay_opll.fur
python ../../json2sms.py -o cmajor_ay_opll.asm -i cmajor ./cmajor_ay_opll.json 

wla-z80 -D BANJO_SYS=2 main.asm
wlalink -s -r linkfile.txt main.rom