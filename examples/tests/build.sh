python ../../furnace2json.py -o ./note_on_off.json ./note_on_off.fur
python ../../json2sms.py -o note_on_off.asm -i note_on_off ./note_on_off.json ret

python ../../furnace2json.py -o ./slides.json ./slides.fur
python ../../json2sms.py -o slides.asm -i slides ./slides.json 

python ../../furnace2json.py -o ./vibrato.json ./vibrato.fur
python ../../json2sms.py -o vibrato.asm -i vibrato ./vibrato.json 

python ../../furnace2json.py -o ./empty_pattern.json ./empty_pattern.fur
python ../../json2sms.py -o empty_pattern.asm -i empty_pattern ./empty_pattern.json 

python ../../furnace2json.py -o ./arpeggios.json ./arpeggios.fur
python ../../json2sms.py -o arpeggios.asm -i arpeggios ./arpeggios.json 

python ../../furnace2json.py -o ./arp_macros.json ./arp_macros.fur
python ../../json2sms.py -o arp_macros.asm -i arp_macros ./arp_macros.json 

python ../../furnace2json.py -o ./noise_modes.json ./noise_modes.fur
python ../../json2sms.py -o noise_modes.asm -i noise_modes ./noise_modes.json

python ../../furnace2json.py -o ./noise_macros.json ./noise_macros.fur
python ../../json2sms.py -o noise_macros.asm -i noise_macros ./noise_macros.json 

python ../../furnace2json.py -o ./detune.json ./detune.fur
python ../../json2sms.py -o detune.asm -i detune ./detune.json 

python ../../furnace2json.py -o ./gg_pan.json ./gg_pan.fur
python ../../json2sms.py -o gg_pan.asm -i gg_pan ./gg_pan.json 

python ../../furnace2json.py -o ./volume_macros.json ./volume_macros.fur
python ../../json2sms.py -o volume_macros.asm -i volume_macros ./volume_macros.json 

wla-z80 -D BANJO_SYS=1 main.asm
wlalink -s -r linkfile.txt tests.sms
wlalink -s -r linkfile.txt tests.gg