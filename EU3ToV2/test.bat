set i=%~n1
set j=%~n2

echo Testing %i% with the %j% configuration
copy "..\EU3_Saves\%i%.zip" ".\%i%.zip"
"%SEVENZIP_LOC%\7z.exe" e -tzip "%i%.zip" "*.*" -mx5
del "%i%.zip"
call EU3toV2Converter.exe "%i%.eu3"
del "%i%.eu3" /q
copy output.v2 "..\testresults\%j%\%i%.v2"
copy log.txt "..\testresults\%j%\%i%-Log.txt"
del output.v2 /q
del log.txt /q