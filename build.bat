del *.zip /q

call "%VSTUDIO_LOC%\VC\vcvarsall.bat"

del Release /s /q
rmdir Release /s /q

del ReleaseIntermediate /s /q
rmdir ReleaseIntermediate /s /q

msbuild.exe CK2ToEU3.sln /p:Configuration=Release
cd Release
del *.pdb
call "%SEVENZIP_LOC%\7z.exe" a -tzip "..\CK2ToEU3.zip" "*.*" -mx5
cd ..