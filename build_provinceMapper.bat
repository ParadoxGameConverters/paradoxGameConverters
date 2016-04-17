set /p version=Version Number?:

call "%VS140COMNTOOLS%\vsvars32.bat"
msbuild.exe "ProvinceMapper\ProvinceMapper.sln" /p:Configuration=Release /m
mkdir "ProvinceMapper-%version%\"
copy "ProvinceMapper\bin\Release\ProvinceMapper.exe" "ProvinceMapper-%version%\ProvinceMapper.exe"
copy "ProvinceMapper\Changelog.txt" "ProvinceMapper-%version%\Changelog.txt"

cd "ProvinceMapper-%version%"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\ProvinceMapper-%version%.zip" "*" -mx5
cd ..
