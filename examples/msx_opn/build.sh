python3 ../../furnace2json.py -o ./cmajor_ay.json ../cmajor_ay.fur
python3 ../../json2sms.py -o cmajor_ay.asm -i cmajor_ay ./cmajor_ay.json

python ../../furnace2json.py -o ./cmajor_opn.json ../cmajor_opn.fur
python ../../json2sms.py -o cmajor_opn.asm -i cmajor_opn ./cmajor_opn.json 

python ../../furnace2json.py -o ./cmajor_opna.json ../cmajor_opna.fur
python ../../json2sms.py -o cmajor_opna.asm -i cmajor_opna ./cmajor_opna.json 

wla-z80 -D BANJO_SYS=2 main.asm
wlalink -s -r linkfile.txt main.rom
