set m=%~n1

echo Preparing to test the %m% config.

mkdir testresults\%m%

copy TestConfigurations\%m%\configuration.txt Release\configuration.txt
copy TestConfigurations\%m%\country_mappings.txt Release\country_mappings.txt
copy TestConfigurations\%m%\culture_mappings.txt Release\culture_mappings.txt
copy TestConfigurations\%m%\religion_mappings.txt Release\religion_mappings.txt

copy test.bat Release\test.bat
cd Release
for /f %%k in ('dir /b ..\CK2_Saves\*.zip') do call test.bat %%k "%m%"
del test.bat /s /q
cd ..