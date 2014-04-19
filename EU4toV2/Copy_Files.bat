copy "Data_Files\changelog.txt" "release\changelog.txt"
copy "Data_Files\configuration.txt" "release\configuration.txt"
copy "Data_Files\readme.txt" "release\readme.txt"
copy "Data_Files\merge_nations.txt" "release\merge_nations.txt"
copy "Data_Files\starting_factories.txt" "release\starting_factories.txt
copy "Data_Files\province_mappings.txt" "release\province_mappings.txt
copy "Data_Files\blocked_nations.txt" "release\blocked_nations.txt
copy "Data_Files\country_mappings.txt" "release\country_mappings.txt
copy "Data_Files\cultureMap.txt" "release\cultureMap.txt
copy "Data_Files\religionMap.txt" "release\religionMap.txt
copy "Data_Files\unions.txt" "release\unions.txt
copy "Data_Files\governmentMapping.txt" "release\governmentMapping.txt
copy "Data_Files\blocked_tech_schools.txt" "release\blocked_tech_schools.txt
copy "Data_Files\leader_traits.txt" "release\leader_traits.txt
copy "Data_Files\regiment_costs.txt" "release\regiment_costs.txt

rem hg log > Release/log.txt
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
mkdir "release\blankMod\output\common"

copy "Data_Files\religion.txt" "release\blankMod\output\common\religion.txt"