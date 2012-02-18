del *.zip /q

call "%VSTUDIO_LOC%\VC\vcvarsall.bat"

del Release /q
rmdir Release

del ReleaseIntermediate/q
rmdir ReleaseIntermediate

msbuild.exe CK2ToEU3.sln /p:Configuration=Release
cd Release
del *.pdb
"%IZARC_LOC%" -ad Release;*
copy ".zip" "../CK2ToEU3.zip"
cd ..