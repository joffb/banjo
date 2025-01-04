sdasz80 -o ../lib/devkitsms/banjo.rel system_sms_devkitsms.inc banjo/banjo.asm
sdasz80 -g -o ../lib/devkitsms/banjo_sn.rel system_sms_devkitsms.inc sn/banjo_sn.asm
sdasz80 -g -o ../lib/devkitsms/banjo_opll.rel system_sms_devkitsms.inc opll/banjo_opll.asm
sdasz80 -g -o ../lib/devkitsms/banjo_opll_drums.rel system_sms_devkitsms.inc opll_drums/banjo_opll_drums.asm

sdasz80 -o ../lib/gbdk_sms/banjo.rel system_sms_gbdk.inc banjo/banjo.asm
sdasz80 -g -o ../lib/gbdk_sms/banjo_sn.rel system_sms_gbdk.inc sn/banjo_sn.asm
sdasz80 -g -o ../lib/gbdk_sms/banjo_opll.rel system_sms_gbdk.inc opll/banjo_opll.asm
sdasz80 -g -o ../lib/gbdk_sms/banjo_opll_drums.rel system_sms_gbdk.inc opll_drums/banjo_opll_drums.asm

sdasz80 -o ../lib/msx/banjo.rel system_msx.inc banjo/banjo.asm
sdasz80 -g -o ../lib/msx/banjo_ay.rel system_msx.inc ay/banjo_ay.asm
sdasz80 -g -o ../lib/msx/banjo_opll.rel system_msx.inc opll/banjo_opll.asm
sdasz80 -g -o ../lib/msx/banjo_opll_drums.rel system_msx.inc opll_drums/banjo_opll_drums.asm

sdasz80 -g -o ../lib/banjo_sfx.rel sfx/banjo_sfx.asm

sdasz80 -g -o ../lib/banjo_queue.rel queue/banjo_queue.asm

# convert the line endings to CRLF so they can be used
# on both Windows and Linux
unix2dos ../lib/*
unix2dos ../lib/devkitsms/*
unix2dos ../lib/gbdk_sms/*
unix2dos ../lib/msx/*

