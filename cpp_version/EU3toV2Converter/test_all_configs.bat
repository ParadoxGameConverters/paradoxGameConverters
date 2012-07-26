echo off

copy Release\configuration.txt Release\configuration-backup.txt
copy Release\country_mappings.txt Release\country_mappings-backup.txt
copy Release\cultureMap.txt Release\cultureMap-backup.txt
copy Release\religionMap.txt Release\religionMap-backup.txt
copy Release\unions.txt Release\unions-backup.txt

for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /q
rmdir testresults

for /f %%n in ('dir /b TestConfigurations\') do call test_all_saves.bat "%%n"

copy Release\configuration-backup.txt Release\configuration.txt
copy Release\country_mappings-backup.txt copy Release\country_mappings.txt
copy Release\cultureMap-backup.txt copy Release\cultureMap.txt
copy Release\religionMap-backup.txt copy Release\religionMap.txt
copy Release\unions-backup.txt copy Release\unions.txt
del Release\configuration-backup.txt /q
del Release\country_mappings-backup.txt /q
del Release\cultureMap-backup.txt /q
del Release\religionMap-backup.txt /q
del Release\unions-backup.txt /q