echo off

copy Release\configuration.txt Release\configuration-backup.txt
copy Release\culture_mappings.txt Release\culture_mappings-backup.txt
copy Release\religion_mappings.txt Release\religion_mappings-backup.txt

for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /s /q
rmdir testresults /s /q

for /f %%n in ('dir /b TestConfigurations\') do call test_all_saves.bat "%%n"

copy Release\configuration-backup.txt Release\configuration.txt
copy Release\culture_mappings-backup.txt copy Release\culture_mappings.txt
copy Release\religion_mappings-backup.txt copy Release\religion_mappings.txt
del Release\configuration-backup.txt /s /q
del Release\culture_mappings-backup.txt /s /q
del Release\religion_mappings-backup.txt /s /q
