echo off

for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /q
rmdir testresults
mkdir testresults\

copy Release\configuration.txt Release\configuration-backup.txt

copy test.bat Release\test.bat
cd Release
for /f %%k in ('dir /b ..\Vic2_Saves\*.zip') do call test.bat "%%k"
xcopy output "..\testresults\%j%" /E /Q /Y /I
del test.bat /q
cd ..

copy Release\configuration-backup.txt Release\configuration.txt
del Release\configuration-backup.txt

cd testresults
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\Vic2toHoI3-testresults.zip" "*.*" -mx5