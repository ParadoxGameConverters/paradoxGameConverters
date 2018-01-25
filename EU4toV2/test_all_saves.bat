echo off

SET SEVENZIP_LOC="C:\Program Files (x86)\7-Zip"
for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /q
rmdir testresults
mkdir testresults\


copy Release\configuration.txt Release\configuration-backup.txt
copy test.bat Release\test.bat
cd Release
for /f %%k in ('dir /b ..\EU4_Saves\*.zip') do call test.bat "%%k"
xcopy output "..\testresults\%j%" /E /Q /Y /I
del test.bat /q
cd ..

copy Release\configuration-backup.txt Release\configuration.txt
del Release\configuration-backup.txt
cd testresults
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU4toV2-testresults.zip" "*.*" -mx5