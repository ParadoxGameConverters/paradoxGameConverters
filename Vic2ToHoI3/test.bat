set i=%~n1

echo Testing %i%
del "configuration.txt"
copy "..\Vic2_Saves\%i%.zip" ".\%i%.zip"
"%SEVENZIP_LOC%\7z.exe" x -tzip "%i%.zip" -mx5 -y
xcopy mod "D:\Paradox Interactive\Victoria 2\mod" /E /Q /Y /I
rmdir mod /s /q
del "%i%.zip"
call V2ToHoI3Converter.exe "%i%.v2"
del "%i%.v2" /q
copy log.txt "..\testresults\%j%\%i%-Log.txt"
del log.txt /q
del %i%.mod
del %i% /q
rmdir %i%