set i=%~n1

echo Testing %i%
del "configuration.txt"
copy "..\EU3_Saves\%i%.zip" ".\%i%.zip"
"%SEVENZIP_LOC%\7z.exe" x -tzip "%i%.zip" -mx5 -y
del "%i%.zip"
call EU3toV2Converter.exe "%i%.eu3"
del "%i%.eu4" /q
copy log.txt "..\testresults\%j%\%i%-Log.txt"
del log.txt /q
del %i%.mod
del %i% /q
rmdir %i%