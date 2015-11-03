set /p version=Version Number?:

call "%VS120COMNTOOLS%\vsvars32.bat"
msbuild.exe "..\paradoxGameConverters - Frontend\Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m
xcopy "..\paradoxGameConverters - Frontend\Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "..\Vic2ToHoI3-%version%"  /Y /E /I

cd Vic2ToHoI3
call "build-VS2013.bat"
xcopy "Release" "..\..\Vic2ToHoI3-%version%\Vic2ToHoI3"  /Y /E /I
cd ..

cd "..\Vic2ToHoI3-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\Vic2ToHoI3-%version%.zip" "*" -mx5
cd paradoxGameConverters - Vic2ToHoI3

