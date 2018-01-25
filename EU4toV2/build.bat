del *.zip /q

REM call "%VS140COMNTOOLS%\vsvars32.bat"
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
SET msBuildLocation="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"

del Release /q
rmdir Release /S /Q

del ReleaseIntermediate/q
rmdir ReleaseIntermediate /S /Q

call %msBuildLocation% EU4ToV2.sln /p:Configuration=Release /m


cd Release
del *.pdb
del *.ipdb
del *.iobj
cd ..