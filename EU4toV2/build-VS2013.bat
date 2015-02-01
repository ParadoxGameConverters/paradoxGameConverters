del *.zip /q

call "%VS120COMNTOOLS%\vsvars32.bat"

del Release /q
rmdir Release /S /Q

del ReleaseIntermediate/q
rmdir ReleaseIntermediate /S /Q

msbuild.exe EU4ToV2-VS2013.sln /p:Configuration=Release /m


cd Release
del *.pdb
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU4ToV2.zip" "*" -mx5
cd ..