echo off

copy Release\configuration.txt Release\configuration-backup.txt
del Release\configuration.txt

for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /q
rmdir testresults

for /f %%n in ('dir /b TestConfigurations\*.txt') do call test_all_saves.bat "%%n"

copy Release\configuration-backup.txt Release\configuration.txt
del Release\configuration-backup.txt /q