del *.zip /q

call "%VS120COMNTOOLS%\vsvars32.bat"

del Release /s /q
rmdir Release /s /q

del ReleaseIntermediate /s /q
rmdir ReleaseIntermediate /s /q

del UI\bin\Release /s /q
rmdir UI\bin\Release /s /q

msbuild.exe CK2ToEU3-VS2013.sln /p:Configuration=Release /m
copy "UI\bin\Release\Converter.UI.exe" ".\release\Converter.UI.exe"
copy "UI\UserPreferences.xml" ".\release\UserPreferences.xml"
copy "UI\Configuration.xml" ".\release\Configuration.xml"

cd Release
del *.pdb
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r "..\CK2ToEU3.zip" "*.*" -mx5
cd ..