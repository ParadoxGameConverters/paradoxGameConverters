rem **Create blankMod**
del "Release\blankMod" /Q /S /F
rmdir "Release\blankMod" /Q /S
xcopy "Data_Files\blankMod" "release\blankmod" /Y /E /I

rem **Copy data files**
copy "Data_Files\configuration.txt" "release\configuration.txt"
copy "Data_Files\configuration.txt" "release\configuration-example.txt"
copy "Data_Files\readme.txt" "release\readme.txt"
copy "Data_Files\merge_nations.txt" "release\merge_nations.txt"
copy "Data_Files\province_mappings.txt" "release\province_mappings.txt
copy "Data_Files\HPM_province_mappings.txt" "release\HPM_province_mappings.txt
copy "Data_Files\country_mappings.txt" "release\country_mappings.txt
copy "Data_Files\governmentMapping.txt" "release\governmentMapping.txt
copy "Data_Files\license.txt" "release\license.txt"
copy "Data_Files\tech_mapping.txt" "release\tech_mapping.txt"
copy "Data_Files\unit_mapping.txt" "release\unit_mapping.txt"
copy "Data_Files\FAQ.txt" "release\FAQ.txt"
copy "Data_Files\resources.txt" "release\resources.txt"
copy "Data_Files\positions.txt" "release\positions.txt"
copy "Data_Files\names.txt" "release\names.txt"
copy "Data_Files\cultureGroupToGraphics.txt" "release\cultureGroupToGraphics.txt"
copy "Data_Files\converterIdeologies.txt" "release\converterIdeologies.txt"
copy "Data_Files\converterLeaderTraits.txt" "release\converterLeaderTraits.txt"
copy "Data_Files\ideologicalAdvisors.txt" "release\ideologicalAdvisors.txt"
copy "Data_Files\ideologicalIdeas.txt" "release\ideologicalIdeas.txt"

rem **Copy flags**
del "Release\flags" /Q /S /F
rmdir "Release\flags" /Q /S
xcopy "Data_Files\flags" "release\flags" /Y /E /I

rem **Create changelog**
del release\changelog.txt
git log --oneline --decorate >> release\makelog.txt
(for /f "delims=" %%i in (release\makelog.txt) do @echo %%i)>release\changelog.txt
del release\makelog.txt
