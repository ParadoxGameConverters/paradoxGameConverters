set m=%~n1

echo Preparing to test the %m% config.

set build_path=%~2

mkdir "%build_path%\testresults\%m%"

copy test.bat "%build_path%\test.bat"
set old_path=%CD%
cd "%build_path%"
for /f %%k in ('dir /b "%old_path%\CK2_Saves\*.zip"') do call test.bat %%k "%m%" "%old_path%"
del test.bat /s /q
cd %old_path%