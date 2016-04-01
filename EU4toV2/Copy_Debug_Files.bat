copy "Data_Files\changelog.txt" "debug\changelog.txt"
copy "Data_Files\configuration.txt" "debug\configuration.txt"
copy "Data_Files\readme.txt" "debug\readme.txt"
copy "Data_Files\merge_nations.txt" "debug\merge_nations.txt"
copy "Data_Files\starting_factories.txt" "debug\starting_factories.txt
copy "Data_Files\province_mappings.txt" "debug\province_mappings.txt
copy "Data_Files\country_mappings.txt" "debug\country_mappings.txt
copy "Data_Files\cultureMap.txt" "debug\cultureMap.txt
copy "Data_Files\religionMap.txt" "debug\religionMap.txt
copy "Data_Files\unions.txt" "debug\unions.txt
copy "Data_Files\governmentMapping.txt" "debug\governmentMapping.txt
copy "Data_Files\blocked_tech_schools.txt" "debug\blocked_tech_schools.txt
copy "Data_Files\leader_traits.txt" "debug\leader_traits.txt
copy "Data_Files\regiment_costs.txt" "debug\regiment_costs.txt
copy "Data_Files\license.txt" "debug\license.txt"
copy "Data_Files\idea_effects.txt" "debug\idea_effects.txt"
copy "Data_Files\colonial.txt" "debug\colonial.txt"
copy "Data_Files\ck2titlemap.txt" "debug\ck2titlemap.txt"

rem hg log > Debug/log.txt
hg log --template "Change:\t\t{rev}: {node}\nAuthor:\t\t{author}\nDescription:\t{desc}\nDate:\t\t{date|isodate}\nBranch:\t\t{branch}\n***\n" > Debug/log.txt
(for /f "delims=" %%i in (debug/log.txt) do @echo %%i)>debug/changelog.txt
del debug\log.txt

del "Debug\blankMod" /Q
rmdir "Debug\blankMod" /S /Q
mkdir "debug\blankMod"
mkdir "debug\blankMod\output"
mkdir "debug\blankMod\output\history"
mkdir "debug\blankMod\output\history\provinces"
mkdir "debug\blankMod\output\history\provinces\africa"
mkdir "debug\blankMod\output\history\provinces\asia"
mkdir "debug\blankMod\output\history\provinces\australia"
mkdir "debug\blankMod\output\history\provinces\austria"
mkdir "debug\blankMod\output\history\provinces\balkan"
mkdir "debug\blankMod\output\history\provinces\canada"
mkdir "debug\blankMod\output\history\provinces\carribean"
mkdir "debug\blankMod\output\history\provinces\central asia"
mkdir "debug\blankMod\output\history\provinces\china"
mkdir "debug\blankMod\output\history\provinces\france"
mkdir "debug\blankMod\output\history\provinces\germany"
mkdir "debug\blankMod\output\history\provinces\india"
mkdir "debug\blankMod\output\history\provinces\indonesia"
mkdir "debug\blankMod\output\history\provinces\italy"
mkdir "debug\blankMod\output\history\provinces\japan"
mkdir "debug\blankMod\output\history\provinces\low countries"
mkdir "debug\blankMod\output\history\provinces\mexico"
mkdir "debug\blankMod\output\history\provinces\pacific island"
mkdir "debug\blankMod\output\history\provinces\portugal"
mkdir "debug\blankMod\output\history\provinces\scandinavia"
mkdir "debug\blankMod\output\history\provinces\south america"
mkdir "debug\blankMod\output\history\provinces\soviet"
mkdir "debug\blankMod\output\history\provinces\spain"
mkdir "debug\blankMod\output\history\provinces\united kingdom"
mkdir "debug\blankMod\output\history\provinces\usa"
mkdir "debug\blankMod\output\history\countries"
mkdir "debug\blankMod\output\history\diplomacy"
mkdir "debug\blankMod\output\history\units"
mkdir "debug\blankMod\output\history\pops"
mkdir "debug\blankMod\output\history\pops\1836.1.1"
mkdir "debug\blankMod\output\common"

xcopy "Data_Files\countries" "debug\blankmod\output\common\countries" /Y /E /I
xcopy "Data_Files\gfx" "debug\blankmod\output\gfx" /Y /E /I
xcopy "Data_files\localisation" "debug\blankmod\output\localisation" /Y /E /I
xcopy "Data_files\interface" "debug\blankmod\output\interface" /Y /E /I
copy "Data_Files\countries.txt" "debug\blankMod\output\common\countries.txt"
copy "Data_Files\country_colors.txt" "debug\blankMod\output\common\country_colors.txt"
copy "Data_Files\religion.txt" "debug\blankMod\output\common\religion.txt"
copy "Data_Files\cultures.txt" "debug\blankMod\output\common\cultures.txt"

