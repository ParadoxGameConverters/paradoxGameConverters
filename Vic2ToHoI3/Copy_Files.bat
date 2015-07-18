copy "Data_Files\changelog.txt" "release\changelog.txt"
copy "Data_Files\configuration.txt" "release\configuration.txt"
copy "Data_Files\readme.txt" "release\readme.txt"
copy "Data_Files\merge_nations.txt" "release\merge_nations.txt"
copy "Data_Files\province_mappings.txt" "release\province_mappings.txt
copy "Data_Files\country_mappings.txt" "release\country_mappings.txt
copy "Data_Files\governmentMapping.txt" "release\governmentMapping.txt
copy "Data_Files\governmentJobs.txt" "release\governmentJobs.txt
copy "Data_Files\leader_traits.txt" "release\leader_traits.txt
copy "Data_Files\license.txt" "release\license.txt"
copy "Data_Files\tech_mapping.txt" "release\tech_mapping.txt"
copy "Data_Files\unit_mapping.txt" "release\unit_mapping.txt"
copy "Data_Files\portraits.txt" "release\portraits.txt"
copy "Data_Files\culture_map.txt" "release\culture_map.txt"

del release\changelog.txt
git log --oneline --decorate >> release/log.txt
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
mkdir "release\blankMod\output\history\provinces\balkan"
mkdir "release\blankMod\output\history\provinces\belgium"
mkdir "release\blankMod\output\history\provinces\canada"
mkdir "release\blankMod\output\history\provinces\carribean"
mkdir "release\blankMod\output\history\provinces\china"
mkdir "release\blankMod\output\history\provinces\east europe"
mkdir "release\blankMod\output\history\provinces\france"
mkdir "release\blankMod\output\history\provinces\germany"
mkdir "release\blankMod\output\history\provinces\holland"
mkdir "release\blankMod\output\history\provinces\italy"
mkdir "release\blankMod\output\history\provinces\japan"
mkdir "release\blankMod\output\history\provinces\Mongolia"
mkdir "release\blankMod\output\history\provinces\portugal"
mkdir "release\blankMod\output\history\provinces\scandinavia"
mkdir "release\blankMod\output\history\provinces\siberia"
mkdir "release\blankMod\output\history\provinces\south america"
mkdir "release\blankMod\output\history\provinces\soviet"
mkdir "release\blankMod\output\history\provinces\spain"
mkdir "release\blankMod\output\history\provinces\united kingdom"
mkdir "release\blankMod\output\history\provinces\usa"
mkdir "release\blankMod\output\history\countries"
mkdir "release\blankMod\output\history\diplomacy"
mkdir "release\blankMod\output\history\units"
mkdir "release\blankMod\output\history\wars"
mkdir "release\blankMod\output\events"
mkdir "release\blankMod\output\decisions"

xcopy "Data_Files\countries" "release\blankmod\output\common\countries" /Y /E /I
xcopy "Data_files\localisation" "release\blankmod\output\localisation" /Y /E /I
xcopy "Data_files\script" "release\blankmod\output\script" /Y /E /I
copy "Data_Files\countries.txt" "release\blankMod\output\common\countries.txt"
copy "Data_Files\country_colors.txt" "release\blankMod\output\common\country_colors.txt"
xcopy "Data_files\wars" "release\blankmod\output\history\wars" /Y /E /I
xcopy "Data_Files\events" "release\blankMod\output\events"
xcopy "Data_Files\decisions" "release\blankMod\output\decisions"