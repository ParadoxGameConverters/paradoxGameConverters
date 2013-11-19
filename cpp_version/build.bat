del *.zip /q

call "%VSTUDIO_LOC%\VC\vcvarsall.bat"

del Release /q
rmdir Release

del ReleaseIntermediate/q
rmdir ReleaseIntermediate

msbuild.exe EU3toV2Converter.sln /p:Configuration=Release
cd Release
del *.pdb
"%IZARC_LOC%" -ad Release;*
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU3ToV2.zip" "*.*" -mx5
cd ..