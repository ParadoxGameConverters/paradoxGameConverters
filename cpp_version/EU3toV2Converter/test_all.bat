del testResults /q
rmdir testresults
mkdir testresults

copy testDW.bat DWToV2\test.bat
cd dwtov2
for /f %%j in ('dir /b ..\..\EU3_DivineWind_Saves\*.zip') do call test.bat %%j
del test.bat /q
cd ..

copy testHttT.bat HttTToV2\test.bat
cd HttTToV2
for /f %%j in ('dir /b ..\..\EU3_HeirToTheThrone_Saves\*.zip') do call test.bat %%j
del test.bat /q
cd ..