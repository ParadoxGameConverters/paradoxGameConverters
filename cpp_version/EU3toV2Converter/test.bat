set i=%~n1
set j=%~n2

echo Testing %i% with the %j% configuration
copy ..\..\EU3_Saves\%i%.zip .\%i%.zip
set PWD=%~dp0
"%IZARC_LOC%" -ef "%PWD%" "%i%.zip"
copy "%i%\*.eu3" .\input.eu3
del %i% /q
del %i%.zip
rmdir %i%
call EU3toV2Converter.exe
del input.eu3 /q
copy output.v2 ..\testresults\%j%\%i%.v2
copy log.txt ..\testresults\%j%\%i%-Log.txt
del output.v2 /q
del log.txt /q