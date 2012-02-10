set m=%~n1

echo Preparing to test the %m% config.

mkdir testresults\%m%

copy TestConfigurations\%m%.txt Release\configuration.txt

copy test.bat Release\test.bat
cd Release
for /f %%k in ('dir /b ..\..\EU3_Saves\*.zip') do call test.bat %%k "%m%"
del test.bat /q
cd ..