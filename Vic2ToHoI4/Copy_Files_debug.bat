rem **Create blankMod**
del "debug\blankMod" /Q /S /F
rmdir "debug\blankMod" /Q /S
xcopy "Data_Files\blankMod" "debug\blankmod" /Y /E /I

rem **Copy data files**
copy "Data_Files\configuration.txt" "debug\configuration.txt"
copy "Data_Files\configuration.txt" "debug\configuration-example.txt"
copy "Data_Files\readme.txt" "debug\readme.txt"
copy "Data_Files\merge_nations.txt" "debug\merge_nations.txt"
copy "Data_Files\province_mappings.txt" "debug\province_mappings.txt
copy "Data_Files\HPM_province_mappings.txt" "debug\HPM_province_mappings.txt
copy "Data_Files\country_mappings.txt" "debug\country_mappings.txt
copy "Data_Files\governmentMapping.txt" "debug\governmentMapping.txt
copy "Data_Files\license.txt" "debug\license.txt"
copy "Data_Files\tech_mapping.txt" "debug\tech_mapping.txt"
copy "Data_Files\unit_mappings.txt" "debug\unit_mappings.txt"
copy "Data_Files\FAQ.txt" "debug\FAQ.txt"
copy "Data_Files\resources.txt" "debug\resources.txt"
copy "Data_Files\positions.txt" "debug\positions.txt"
copy "Data_Files\names.txt" "debug\names.txt"
copy "Data_Files\cultureGroupToGraphics.txt" "debug\cultureGroupToGraphics.txt"
copy "Data_Files\converterIdeologies.txt" "debug\converterIdeologies.txt"
copy "Data_Files\converterLeaderTraits.txt" "debug\converterLeaderTraits.txt"
copy "Data_Files\ideologicalAdvisors.txt" "debug\ideologicalAdvisors.txt"
copy "Data_Files\ideologicalDecisions.txt" "debug\ideologicalDecisions.txt"
copy "Data_Files\ideologicalIdeas.txt" "debug\ideologicalIdeas.txt"
copy "Data_Files\converterIdeas.txt" "debug\converterIdeas.txt"
copy "Data_Files\converterFocuses.txt" "debug\converterFocuses.txt"
copy "Data_Files\ai_peaces.txt" "debug\ai_peaces.txt"

rem **Copy flags**
rem rem del "debug\flags" /Q /S /F
rem rmdir "debug\flags" /Q /S
rem xcopy "Data_Files\flags" "debug\flags" /Y /E /I

rem **Create changelog**
del debug\changelog.txt
git log --oneline --decorate >> debug\makelog.txt
(for /f "delims=" %%i in (debug\makelog.txt) do @echo %%i)>debug\changelog.txt
del debug\makelog.txt
