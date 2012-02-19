set i=%~n1
set j=%~n2

echo Testing %i% with the %j% configuration
copy ..\CK2_Saves\%i%.zip .\%i%.zip
set PWD=%~dp0
"%IZARC_LOC%" -ef "%PWD%" "%i%.zip"
copy "%i%\*.ck2" .\input.ck2
del %i% /q
del %i%.zip
rmdir %i%
call CK2ToEU3.exe
del input.ck2 /q
copy output.eu3 ..\testresults\%j%\%i%.eu3
copy log.txt ..\testresults\%j%\%i%-Log.txt
del output.eu3 /q
del log.txt /q