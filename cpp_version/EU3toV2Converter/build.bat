del *.zip /q

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

del dwtov2 /q
rmdir dwtov2

del DWintermediate /q
rmdir DWintermediate

msbuild.exe EU3toV2Converter.sln /p:Configuration=DWToV2
cd DWToV2
D:\IZArc\IZArc.exe -ad DWToV2;*
copy ".zip" "../DWToV2.zip"
cd ..


del HttTtov2 /q
rmdir HttTtov2

del HttTintermediate /q
rmdir HttTintermediate

msbuild.exe EU3toV2Converter.sln /p:Configuration=HttTToV2
cd HttTToV2
D:\IZArc\IZArc.exe -ad HttTToV2;*
copy ".zip" "../HttTToV2.zip"
cd ..