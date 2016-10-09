set /p version=Version Number?:

call "%VS140COMNTOOLS%\vsvars32.bat"
msbuild.exe "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m
xcopy "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "EU4ToVic2-%version%"  /Y /E /I
copy "EU4toV2\Data_Files\Eu4ToVic2DefaultConfiguration.xml" "EU4ToVic2-%version%\Configuration\"
copy "EU4toV2\Data_Files\SupportedConvertersDefault.xml" "EU4ToVic2-%version%\Configuration\"
cd "EU4ToVic2-%version%\"
del *.pdb
cd ..

cd EU4toV2
call "build.bat"
xcopy "Release" "..\EU4ToVic2-%version%\EU4ToVic2"  /Y /E /I
copy "%VS140COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC140.CRT\msvcp140.dll" "..\EU4ToVic2-%version%\Vic2ToHoI4\msvcp140.dll"
copy "%VS140COMNTOOLS%\..\..\VC\redist\x86\Microsoft.VC140.CRT\vcruntime140.dll" "..\EU4ToVic2-%version%\Vic2ToHoI4\vcruntime140.dll"
cd ..

cd "EU4ToVic2-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU4ToV2-%version%.zip" "*" -mx5
cd ..
