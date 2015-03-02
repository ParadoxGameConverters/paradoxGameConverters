copy "data files\country_mappings.txt" "release\country_mappings.txt"
copy "data files\readme.txt" "release\readme.txt"
copy "data files\province_mappings - TESB.txt" "release\province_mappings.txt"
copy "data files\cultureMap - TESB.txt" "release\cultureMap.txt"
copy "Data_Files\slaveCultureMap.txt" "release\slaveCultureMap.txt"
copy "data files\religionMap - TESB.txt" "release\religionMap.txt"
copy "data files\unions.txt" "release\unions.txt"
copy "data files\governmentMapping.txt" "release\governmentMapping.txt"
copy "data files\port_whitelist.txt" "release\port_whitelist.txt"
copy "data files\port_blacklist.txt" "release\port_blacklist.txt"
copy "data files\starting_factories.txt" "release\starting_factories.txt"
copy "data files\merge_nations.txt" "release\merge_nations.txt"
copy "data files\configuration.txt" "release\configuration.txt"
copy "data files\blocked_tech_schools.txt" "release\blocked_tech_schools.txt"
copy "Data_Files\leader_traits.txt" "release\leader_traits.txt
copy "Data_Files\regiment_costs.txt" "release\regiment_costs.txt
copy "Data_Files\license.txt" "release\license.txt"

hg log --template "Change:\t\t{rev}: {node}\nAuthor:\t\t{author}\nDescription:\t{desc}\nDate:\t\t{date|isodate}\nBranch:\t\t{branch}\n***\n" > Release/log.txt
(for /f "delims=" %%i in (release/log.txt) do @echo %%i)>release/changelog.txt
del release\log.txt

del "Release\blankMod" /Q
xcopy "Data Files\blankMod - TESB" "release\blankmod" /Y /E /I

xcopy "Data Files\countries" "release\blankmod\output\common\countries" /Y /E /I
xcopy "Data Files\gfx" "release\blankmod\output\gfx" /Y /E /I
xcopy "Data Files\interface" "release\blankmod\output\interface" /Y /E /I
xcopy "Data files\localisation" "release\blankmod\output\localisation" /Y /E /I
copy "Data Files\countries.txt" "release\blankMod\output\common\countries.txt"
copy "Data Files\country_colors.txt" "release\blankMod\output\common\country_colors.txt"
copy "Data Files\religion.txt" "release\blankMod\output\common\religion.txt"
copy "Data Files\cultures.txt" "release\blankMod\output\common\cultures.txt"
