set /p version=Version Number?:

call "%VS140COMNTOOLS%\vsvars32.bat"
msbuild.exe "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend.sln" /p:Configuration=Release /m
xcopy "Frontend\ParadoxConverters.Frontend\ParadoxConverters.Frontend\bin\Release" "Vic2ToHoI3-%version%"  /Y /E /I
copy "Vic2ToHoI3\Data_Files\Vic2toHoI3DefaultConfiguration.xml" "Vic2ToHoI3-%version%\Configuration\"

cd Vic2ToHoI3
call "build.bat"
xcopy "Release" "..\Vic2ToHoI3-%version%\Vic2ToHoI3"  /Y /E /I
cd ..

cd "Vic2ToHoI3-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\Vic2ToHoI3-%version%.zip" "*" -mx5
cd ..

