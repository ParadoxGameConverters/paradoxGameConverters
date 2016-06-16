set /p version=Version Number?:

call "%VS140COMNTOOLS%\vsvars32.bat"
msbuild.exe "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m
xcopy "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "Vic2ToHoI4-%version%"  /Y /E /I
copy "Vic2ToHoI4\Data_Files\Vic2toHoI4DefaultConfiguration.xml" "Vic2ToHoI4-%version%\Configuration\"
copy "Vic2ToHoI4\Data_Files\SupportedConvertersDefault.xml" "Vic2ToHoI4-%version%\Configuration\"

cd Vic2ToHoI4
call "build.bat"
xcopy "Release" "..\Vic2ToHoI4-%version%\Vic2ToHoI4"  /Y /E /I
copy "%VS140COMNTOOLS%\..\..\VC\redist\x64\Microsoft.VC140.CRT\msvcp140.dll" "..\Vic2ToHoI4-%version%\Vic2ToHoI4\msvcp140.dll"
cd ..

cd "Vic2ToHoI4-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\Vic2ToHoI4-%version%.zip" "*" -mx5
cd ..

