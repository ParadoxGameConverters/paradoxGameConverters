del *.zip /q

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

del Release /q
rmdir Release /S /Q

del ReleaseIntermediate/q
rmdir ReleaseIntermediate /S /Q

msbuild.exe Vic2ToHoI4.sln /p:Configuration=Release /p:Platform=Win32 /m


cd Release
del *.pdb
del *.ipdb
del *.iobj
cd ..