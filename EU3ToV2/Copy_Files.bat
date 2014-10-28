copy "data files\country_mappings.txt" "release\country_mappings.txt"
copy "data files\readme.txt" "release\readme.txt"
copy "data files\province_mappings.txt" "release\province_mappings.txt"
copy "data files\cultureMap.txt" "release\cultureMap.txt"
copy "data files\religionMap.txt" "release\religionMap.txt"
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
rmdir "Release\blankMod" /S /Q
mkdir "release\blankMod"
mkdir "release\blankMod\output"
mkdir "release\blankMod\output\history"
mkdir "release\blankMod\output\history\provinces"
mkdir "release\blankMod\output\history\provinces\africa"
mkdir "release\blankMod\output\history\provinces\asia"
mkdir "release\blankMod\output\history\provinces\australia"
mkdir "release\blankMod\output\history\provinces\austria"
mkdir "release\blankMod\output\history\provinces\balkan"
mkdir "release\blankMod\output\history\provinces\canada"
mkdir "release\blankMod\output\history\provinces\carribean"
mkdir "release\blankMod\output\history\provinces\central asia"
mkdir "release\blankMod\output\history\provinces\china"
mkdir "release\blankMod\output\history\provinces\france"
mkdir "release\blankMod\output\history\provinces\germany"
mkdir "release\blankMod\output\history\provinces\india"
mkdir "release\blankMod\output\history\provinces\indonesia"
mkdir "release\blankMod\output\history\provinces\italy"
mkdir "release\blankMod\output\history\provinces\japan"
mkdir "release\blankMod\output\history\provinces\low countries"
mkdir "release\blankMod\output\history\provinces\mexico"
mkdir "release\blankMod\output\history\provinces\pacific island"
mkdir "release\blankMod\output\history\provinces\portugal"
mkdir "release\blankMod\output\history\provinces\scandinavia"
mkdir "release\blankMod\output\history\provinces\south america"
mkdir "release\blankMod\output\history\provinces\soviet"
mkdir "release\blankMod\output\history\provinces\spain"
mkdir "release\blankMod\output\history\provinces\united kingdom"
mkdir "release\blankMod\output\history\provinces\usa"
mkdir "release\blankMod\output\history\countries"
mkdir "release\blankMod\output\history\diplomacy"
mkdir "release\blankMod\output\history\units"
mkdir "release\blankMod\output\history\pops"
mkdir "release\blankMod\output\history\pops\1836.1.1"
mkdir "release\blankMod\output\common"

xcopy "Data Files\countries" "release\blankmod\output\common\countries" /Y /E /I
xcopy "Data Files\gfx" "release\blankmod\output\gfx" /Y /E /I
xcopy "Data files\localisation" "release\blankmod\output\localisation" /Y /E /I
copy "Data Files\countries.txt" "release\blankMod\output\common\countries.txt"
copy "Data Files\country_colors.txt" "release\blankMod\output\common\country_colors.txt"
copy "Data Files\religion.txt" "release\blankMod\output\common\religion.txt"
copy "Data Files\cultures.txt" "release\blankMod\output\common\cultures.txt"
