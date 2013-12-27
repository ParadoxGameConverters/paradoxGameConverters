set i=%~n1
set j=%~n2

echo Testing %i% with the %j% configuration
copy "..\EU4_Saves\%i%.zip" ".\%i%.zip"
"%SEVENZIP_LOC%\7z.exe" e -tzip "%i%.zip" "*.*" -mx5
del "%i%.zip"
call EU4toV2Converter.exe "%i%.eu4"
del "%i%.eu4" /q
xcopy output "..\testresults\%j%\%i%" /E /Q /Y /I
copy log.txt "..\testresults\%j%\%i%-Log.txt"
del output /q
rmdir output /S /Q
del log.txt /q