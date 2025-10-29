for filename in ../music_driver/banjo/*.inc; do
    node convert_wladx_sdas.js "../music_driver/banjo/$(basename $filename)" "banjo/$(basename $filename)"
done

for filename in ../music_driver/ay/*.inc; do
    node convert_wladx_sdas.js "../music_driver/ay/$(basename $filename)" "ay/$(basename $filename)"
done


for filename in ../music_driver/sn/*.inc; do
    node convert_wladx_sdas.js "../music_driver/sn/$(basename $filename)" "sn/$(basename $filename)"
done

for filename in ../music_driver/opll/*.inc; do
    node convert_wladx_sdas.js "../music_driver/opll/$(basename $filename)" "opll/$(basename $filename)"
done

for filename in ../music_driver/opll_drums/*.inc; do
    node convert_wladx_sdas.js "../music_driver/opll_drums/$(basename $filename)" "opll_drums/$(basename $filename)"
done

for filename in ../music_driver/sfx/*.inc; do
    node convert_wladx_sdas.js "../music_driver/sfx/$(basename $filename)" "sfx/$(basename $filename)"
done