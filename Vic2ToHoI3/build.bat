del *.zip /q

call "%VS140COMNTOOLS%\vsvars32.bat"

del Release /q
rmdir Release /S /Q

del ReleaseIntermediate/q
rmdir ReleaseIntermediate /S /Q

msbuild.exe V2ToHoI3.sln /p:Configuration=Release /m


cd Release
del *.pdb
del *.ipdb
del *.iobj
cd ..