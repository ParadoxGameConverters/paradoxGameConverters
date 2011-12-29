del testResults /q
rmdir testresults
mkdir testresults

copy test.bat Release\test.bat
cd Release
for /f %%j in ('dir /b ..\..\EU3_Saves\*.zip') do call test.bat %%j
del test.bat /q
cd ..