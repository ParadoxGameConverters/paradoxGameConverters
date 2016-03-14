--Modified for randomhoi

-----------------------------------------------------------
-- NOTES: This file is run on app start after exports are done inside 
-- 		  the engine (once per context created)
-----------------------------------------------------------

-- set up path
package.path = package.path .. ";script\\?.lua;script\\country\\?.lua"
package.path = package.path .. ";common\\?.lua"

--require('hoi') -- already imported by game, contains all exported classes
require('utils')
require('defines')
require('ai_country')
require('ai_foreign_minister')
require('ai_intelligence_minister')
require('ai_politics_minister')
require('ai_production_minister')
require('ai_support_functions')
require('ai_tech_minister')
require('ai_trade')
require('ai_license')

-- Default Files
require('DEFAULT_LAND')
require('DEFAULT_MIXED')
require('DEFAULT_ALL')

-- load country specific AI modules.

