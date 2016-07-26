del "Release\blankMod" /Q
rmdir "Release\blankMod" /S /Q
mkdir "release\blankMod"
mkdir "release\blankMod\output"
mkdir "release\blankMod\output\gfx"
mkdir "release\blankMod\output\gfx\interface"
mkdir "release\blankMod\output\history"
mkdir "release\blankMod\output\interface"
mkdir "release\blankMod\output\history\provinces"
mkdir "release\blankMod\output\history\countries"

copy "Data_Files\configuration.txt" "release\configuration.txt"
copy "Data_Files\readme.txt" "release\readme.txt"
copy "Data_Files\merge_nations.txt" "release\merge_nations.txt"
copy "Data_Files\province_mappings.txt" "release\province_mappings.txt
copy "Data_Files\country_mappings.txt" "release\country_mappings.txt
copy "Data_Files\governmentMapping.txt" "release\governmentMapping.txt
copy "Data_Files\license.txt" "release\license.txt"
copy "Data_Files\tech_mapping.txt" "release\tech_mapping.txt"
copy "Data_Files\unit_mapping.txt" "release\unit_mapping.txt"
copy "Data_Files\culture_map.txt" "release\culture_map.txt"
copy "Data_Files\FAQ.txt" "release\FAQ.txt"
copy "Data_Files\resources.txt" "release\resources.txt"
copy "Data_Files\navalprovinces.txt" "release\navalprovinces.txt"
copy "Data_Files\positions.txt" "release\positions.txt"
copy "Data_Files\adj.txt" "release\adj.txt"
copy "Data_Files\NeededFiles\countrypoliticsview.gfx" "release\blankmod\output\interface\countrypoliticsview.gfx"

xcopy "Data_Files\events" "release\blankmod\output\events" /Y /E /I
xcopy "Data_Files\bookmarks" "release\blankmod\output\common\bookmarks" /Y /E /I
xcopy "Data_Files\on_actions" "release\blankmod\output\common\on_actions" /Y /E /I
rem xcopy "Data_Files\flags" "release\flags" /Y /E /I
xcopy "Data_Files\NeededFiles\interface" "release\blankmod\output\gfx\interface" /Y /E /I
xcopy "Data_Files\NeededFiles\ideologies" "release\blankmod\output\ideologies" /Y /E /I
xcopy "Data_Files\NeededFiles\defines" "release\blankmod\output\defines" /Y /E /I

del release\changelog.txt
git log --oneline --decorate >> release\makelog.txt
(for /f "delims=" %%i in (release\makelog.txt) do @echo %%i)>release\changelog.txt
del release\makelog.txt
