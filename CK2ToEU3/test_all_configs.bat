echo off

mkdir testresults

copy "%1\configuration.txt" "%1\configuration-backup.txt"
copy "%1\culture_mappings.txt" "%1\culture_mappings-backup.txt"
copy "%1\religion_mappings.txt" "%1\religion_mappings-backup.txt"

for /f %%p in ('dir /b testresults\') do call del testresults\%%p /q
for /f %%p in ('dir /b testresults\') do call rmdir testresults\%%p
del testResults /s /q
rmdir testresults /s /q

for /f %%n in ('dir /b TestConfigurations\') do call test_all_saves.bat "%%n" "%1"

copy "%1\configuration-backup.txt" "%1\configuration.txt"
copy "%1\culture_mappings-backup.txt" %1\culture_mappings.txt"
copy "%1\religion_mappings-backup.txt" "%1\religion_mappings.txt"
del "%1\configuration-backup.txt" /s /q
del "%1\culture_mappings-backup.txt" /s /q
del "%1\religion_mappings-backup.txt" /s /q

call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "testresults.zip" "testresults\*.*" -mx5