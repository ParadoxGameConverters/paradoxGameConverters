set m=%~n1

echo Preparing to test the %m% config.

mkdir testresults\%m%

copy TestConfigurations\%m%\configuration.txt Release\configuration.txt
copy TestConfigurations\%m%\country_mappings.txt Release\country_mappings.txt
copy TestConfigurations\%m%\cultureMap.txt Release\cultureMap.txt
copy TestConfigurations\%m%\religionMap.txt Release\religionMap.txt
copy TestConfigurations\%m%\unions.txt Release\unions.txt

copy test.bat Release\test.bat
cd Release
for /f %%k in ('dir /b ..\..\EU3_Saves\*.zip') do call test.bat "%%k" "%m%"
del test.bat /q
cd ..