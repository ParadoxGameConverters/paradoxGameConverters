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

-- load country specific AI modules.
-- Majors
require('GER')
require('ENG')
require('SOV')
require('USA')
require('ITA')
require('JAP')
require('FRA')

-- Minors (Alphabetized)
require('AST')
require('BEL')
require('BRA')
require('BUL')
require('CAN')
require('CGX')
require('CHC')
require('CHI')
require('CSX')
require('CXB')
require('CYN')
require('DEN')
require('FIN')
require('GRE')
require('HOL')
require('HUN')
require('LUX')
require('MAN')
require('MEN')
require('MEX')
require('NZL')
require('PER')
require('POL')
require('POR')
require('ROM')
require('SAF')
require('SCH')
require('SIA')
require('SIK')
require('SPA')
require('SPR')
require('SWE')
require('TIB')
require('TUR')
require('VIC')
require('YUG')
