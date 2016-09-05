set i=%~n1
set j=%~n2
set old_path=%~3

echo Testing %i% with the %j% configuration
copy "%old_path%\CK2_Saves\%i%.zip" .\%i%.zip

"%SEVENZIP_LOC%\7z.exe" e -tzip "%i%.zip" "*.*" -mx5
del %i%.zip
call ConverterApp.exe %i%.ck2
del %i%.ck2 /q 
copy %i%.eu3 "testresults\%j%\"
del %i%.eu3 /q
copy log.txt "testresults\%j%\%i%-Log.txt"
del log.txt /q