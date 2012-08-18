del *.zip /q

call "%VSTUDIO_LOC%\VC\vcvarsall.bat"

del Release /q
rmdir Release

del ReleaseIntermediate/q
rmdir ReleaseIntermediate

msbuild.exe CK2ToEU3.sln /p:Configuration=Release
cd Release
del *.pdb
call "%SEVENZIP_LOC%\7z.exe" a -tzip "..\CK2ToEU3.zip" "*.*" -mx5
cd ..