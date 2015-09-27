set /p version=Version Number?:

call "%VS120COMNTOOLS%\vsvars32.bat"
msbuild.exe "..\paradoxGameConverters - Frontend\Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m
xcopy "..\paradoxGameConverters - Frontend\Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "..\EU4ToVic2-%version%"  /Y /E /I

cd EU4toV2
call "build-VS2013.bat"
xcopy "Release" "..\..\EU4ToVic2-%version%\EU4ToVic2"  /Y /E /I
cd ..

cd "..\EU4ToVic2-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\EU4ToV2-%version%.zip" "*" -mx5
cd paradoxGameConverters - EU4ToVic2

