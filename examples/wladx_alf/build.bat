python ..\..\furnace2json.py -o .\alf_song.json ..\alf_cmajor.fur
python ..\..\json2sms.py -o alf_song.asm -i alf_song .\alf_song.json 

wla-z80 -D BANJO_SYS=9 main.asm
wlalink -s -r linkfile.txt alf_banjo.sms