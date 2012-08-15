set i=%~n1
set j=%~n2

echo Testing %i% with the %j% configuration
copy ..\CK2_Saves\%i%.zip .\%i%.zip
set PWD=%~dp0
"%IZARC_LOC%" -ef "%PWD%" "%i%.zip"
copy "%i%\*.ck2" .\%i%.ck2
del %i% /q
del %i%.zip
rmdir %i%
call CK2ToEU3.exe %i%.ck2
del *.ck2 /q
copy *.eu3 ..\testresults\%j%\
del *.eu3 /q
copy log.txt ..\testresults\%j%\%i%-Log.txt
del log.txt /q