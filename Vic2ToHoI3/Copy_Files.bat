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
copy "Data_Files\personality_map.txt" "release\personality_map.txt"
copy "Data_Files\background_map.txt" "release\background_map.txt"
copy "Data_Files\FAQ.txt" "release\FAQ.txt"
copy "Data_Files\airTEMPLATE.lua" "release\airTEMPLATE.lua"
copy "Data_Files\autoexecTEMPLATE.lua" "release\autoexecTEMPLATE.lua"
copy "Data_Files\infatryTEMPLATE.lua" "release\infatryTEMPLATE.lua"
copy "Data_Files\shipTemplate.lua" "release\shipTemplate.lua"
copy "Data_Files\tankTemplate.lua" "release\tankTemplate.lua"
copy "Data_Files\ai_focus.txt" "release\ai_focus.txt"

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
mkdir "release\blankMod\output\history\countries"
mkdir "release\blankMod\output\history\diplomacy"
mkdir "release\blankMod\output\history\leaders"
mkdir "release\blankMod\output\history\units"
mkdir "release\blankMod\output\history\wars"
mkdir "release\blankMod\output\events"
mkdir "release\blankMod\output\decisions"
mkdir "release\blankMod\output\script"
mkdir "release\blankMod\output\script\country"

xcopy "Data_Files\history\provinces" "release\blankmod\output\history\provinces" /Y /E /I
xcopy "Data_Files\countries" "release\blankmod\output\common\countries" /Y /E /I
xcopy "Data_files\localisation" "release\blankmod\output\localisation" /Y /E /I
xcopy "Data_files\script" "release\blankmod\output\script" /Y /E /I
copy "Data_Files\countries.txt" "release\blankMod\output\common\countries.txt"
copy "Data_Files\country_colors.txt" "release\blankMod\output\common\country_colors.txt"
xcopy "Data_files\wars" "release\blankmod\output\history\wars" /Y /E /I
xcopy "Data_Files\events" "release\blankMod\output\events"
xcopy "Data_Files\decisions" "release\blankMod\output\decisions"