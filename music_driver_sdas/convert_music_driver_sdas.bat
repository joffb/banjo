for /F %%G in ('dir /b ..\music_driver\banjo\*.inc') do node convert_wladx_sdas.js ..\music_driver\banjo\%%G banjo\%%G
for /F %%G in ('dir /b ..\music_driver\ay\*.inc') do node convert_wladx_sdas.js ..\music_driver\ay\%%G ay\%%G
for /F %%G in ('dir /b ..\music_driver\opll\*.inc') do node convert_wladx_sdas.js ..\music_driver\opll\%%G opll\%%G
for /F %%G in ('dir /b ..\music_driver\opll_drums\*.inc') do node convert_wladx_sdas.js ..\music_driver\opll_drums\%%G opll_drums\%%G
for /F %%G in ('dir /b ..\music_driver\sn\*.inc') do node convert_wladx_sdas.js ..\music_driver\sn\%%G sn\%%G
for /F %%G in ('dir /b ..\music_driver\queue\*.inc') do node convert_wladx_sdas.js ..\music_driver\queue\%%G queue\%%G
for /F %%G in ('dir /b ..\music_driver\sfx\*.inc') do node convert_wladx_sdas.js ..\music_driver\sfx\%%G sfx\%%G