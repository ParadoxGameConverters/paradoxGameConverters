del "*.zip" /q

del "%1\*.pdb"
call "%SEVENZIP_LOC%\7z.exe" a -tzip -r ".\CK2ToEU3.zip" "%1\*.*" -mx5