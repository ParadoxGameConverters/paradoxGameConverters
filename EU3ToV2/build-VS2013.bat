del *.zip /q

call "%VS120COMNTOOLS%\vsvars32.bat"

del Release /q
rmdir Release /S /Q

del ReleaseIntermediate/q
rmdir ReleaseIntermediate

msbuild.exe EU3toV2Converter-VS2013.sln /p:Configuration=Release /m


cd Release
del *.pdb
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU3ToV2.zip" "*" -mx5
cd ..