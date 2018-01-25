set /p version=Version Number?:

REM call "%VS140COMNTOOLS%\vsvars32.bat"
SET msBuildLocation=C:\PROGRA~2\MIB055~1\2017\COMMUN~1\MSBuild\15.0\Bin
SET toolsLocation="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.10.25008\x86\Microsoft.VC150.CRT"

SET SEVENZIP_LOC=""C:\Program Files (x86)\7-Zip""
"%msBuildLocation%\msbuild.exe" "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m 
xcopy "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "EU4ToVic2-%version%"  /Y /E /I
copy "EU4toV2\Data_Files\Eu4ToVic2DefaultConfiguration.xml" "EU4ToVic2-%version%\Configuration\"
copy "EU4toV2\Data_Files\SupportedConvertersDefault.xml" "EU4ToVic2-%version%\Configuration\"
cd "EU4ToVic2-%version%\"
del *.pdb
cd ..

cd EU4toV2
call "build.bat"
xcopy "Release" "..\EU4ToVic2-%version%\EU4ToVic2"  /Y /E /I
copy "%toolsLocation%\msvcp140.dll" "..\EU4ToVic2-%version%\EU4ToVic2\msvcp140.dll"
copy "%toolsLocation%\vcruntime140.dll" "..\EU4ToVic2-%version%\EU4ToVic2\vcruntime140.dll"
cd ..

cd "EU4ToVic2-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU4ToV2-%version%.zip" "*" -mx5
cd ..
