sdasz80 -o lib/music_driver_fm.rel mode/fm.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_sn.rel mode/sn.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_sn_fm.rel mode/sn_fm.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_fm_drums.rel mode/fm_drums.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_dual_sn.rel mode/dual_sn.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_sn_fm_drums.rel mode/sn_fm_drums.asm music_driver_sdas.asm

sdasz80 -o lib/music_driver_qo_fm.rel mode/queues_off.asm mode/fm.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_qo_sn.rel mode/queues_off.asm mode/sn.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_qo_sn_fm.rel mode/queues_off.asm mode/sn_fm.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_qo_fm_drums.rel mode/queues_off.asm mode/fm_drums.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_qo_dual_sn.rel mode/queues_off.asm mode/dual_sn.asm music_driver_sdas.asm
sdasz80 -o lib/music_driver_qo_sn_fm_drums.rel mode/queues_off.asm mode/sn_fm_drums.asm music_driver_sdas.asm