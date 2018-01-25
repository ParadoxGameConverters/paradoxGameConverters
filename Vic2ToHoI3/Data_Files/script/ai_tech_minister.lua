
-- Techs that are used in the main file to be ignored
--   techname|level (level must be 1-9 a 0 means ignore all
--   these local variables can be overiden in the country specific files
--   use as the first tech name the word "all" and it will cause the AI to ignore all the techs

-- Index IDs for the main Research areas
local _RESEARCH_LAND_ = 1
local _RESEARCH_LAND_DOC_ = 2
local _RESEARCH_AIR_ = 3
local _RESEARCH_AIR_DOC_ = 4
local _RESEARCH_NAVAL_ = 5
local _RESEARCH_NAVAL_DOC_ = 6
local _RESEARCH_INDUSTRIAL_ = 7
local _RESEARCH_SECRET_ = 8
local _RESEARCH_UNKNOWN_ = 9

local TechnologyData = {}

-- ###################################
-- # Main Method called by the EXE
-- #####################################
function TechMinister_Tick(minister, vbSliders, vbResearch)
	-- Reset Global Array Container
	TechnologyData = {
		minister = minister,
		ministerAI = minister:GetOwnerAI(),
		ministerTag = minister:GetCountryTag(),
		ministerCountry = nil,
		humanTag = CCurrentGameState.GetPlayer(), 
		IsAtWar = nil, -- Boolean are they atwar with someone
		IsMajor = nil, -- Boolean are they a major power
		IsNaval = nil, -- Boolean do they have the min requirements to build ships
		TechStatus = nil, -- TechStatus Object
		icTotal = 0, -- Total Amount of IC they have
		PortsTotal = 0, -- Total amount of ports
		AirfieldsTotal = 0} -- Total amount of airfields

			
	if math.random(2) == 1 then
	
		

	-- Initialize Production Object 
	--   only the ones that are used for the slider
	-- #################
	TechnologyData.ministerCountry = TechnologyData.ministerTag:GetCountry()
	TechnologyData.IsMajor = TechnologyData.ministerCountry:IsMajor()
	-- End Initialize Production Object
	-- #################	

	local ResearchSlotsAllowed = 0
	
	if vbSliders then
		-- Calling balance sliders like this allows me to get what the new Research slot count would be
		--    once the sliders are shifted
		local loLeaderSliders = BalanceLeadershipSliders(TechnologyData, vbSliders)
		ResearchSlotsAllowed = math.ceil(loLeaderSliders.Slots_Research)
	else
		-- Sliders already set by player
		ResearchSlotsAllowed = TechnologyData.ministerCountry:GetAllowedResearchSlots()
	end
	
	if vbResearch then
		local ResearchSlotsNeeded = ResearchSlotsAllowed - TechnologyData.ministerCountry:GetNumberOfCurrentResearch()
		-- Performance check, exit if there are no slots available
		if ResearchSlotsNeeded >= 0.01 then
			-- Initialize Production Object
			--   add the ones used for Tech Research
			-- #################
			TechnologyData.TechStatus = TechnologyData.ministerCountry:GetTechnologyStatus()
			TechnologyData.PortsTotal = TechnologyData.ministerCountry:GetNumOfPorts()
			TechnologyData.AirfieldsTotal = TechnologyData.ministerCountry:GetNumOfAirfields()
			TechnologyData.icTotal = TechnologyData.ministerCountry:GetTotalIC()
			TechnologyData.IsAtWar = TechnologyData.ministerCountry:IsAtWar()
			TechnologyData.IsNaval = (TechnologyData.PortsTotal > 0 and TechnologyData.icTotal >= 20)
			-- End Initialize Production Object
			-- #################	

			local liMaxTechYear = CTechnologyDataBase.GetLatestTechYear() + 1
			Process_Tech((CCurrentGameState.GetCurrentDate():GetYear()), liMaxTechYear, ResearchSlotsAllowed, ResearchSlotsNeeded)
		end
	end
	
		end
end

-- Balances the research sliders	
-- NOTE: This method is called from the following files
--     ai_foreign_minister.lua
--     ai_tech_minister.lua
function BalanceLeadershipSliders(StandardDataObject, vbSliders)
	local liInfluenceCap = 25 -- Cap based on total leadership, if below this do not influence at all
	local liDiplomacyNoFaction = 0.5 -- Major or Minor not in a faction or does not meet influence cap
	local liDiplomacyInFaction = 4.5 -- Majors that are in a faction and exceed influence cap
	
    local Leadership = {
		NCONeeded = false,
		CanInfluence = false,
		ActiveInfluence = StandardDataObject.ministerCountry:CalculateNumberOfActiveInfluences(),
		Diplomats = StandardDataObject.ministerCountry:GetDiplomaticInfluence():Get(),
		TotalLeadership = StandardDataObject.ministerCountry:GetTotalLeadership():Get(),
		Default_Research = 0,
		Default_Espionage = 0.03,
		Default_Diplomacy = 0.08,
		Default_NCO = 0.1,
		Percent_Research = 0,
		Percent_Espionage = 0.03,
		Percent_Diplomacy = 0.08,
		Percent_NCO = 0.1,
		Slots_Research = 0,
		Slots_Espionage = 0,
		Slots_Diplomacy = 0,
		Slots_NCO = 0}
	
	Leadership.CanInfluence = (StandardDataObject.ministerCountry:HasFaction() and Leadership.TotalLeadership >= liInfluenceCap)
	
	-- Officer ratio.
	local officer_ratio = StandardDataObject.ministerCountry:GetOfficerRatio():Get()
		
	-- Checks to see if you are loosing officers
	--   if so take them from espionage and diplomacy
	if officer_ratio < 1.0 then
		-- Move the Espionage into the NCO and set it to 0 since we are short
		Leadership.Percent_NCO = 0.5 + Leadership.Percent_Espionage
		Leadership.Percent_Espionage = 0.0
		Leadership.NCONeeded = true
	elseif officer_ratio < 1.05 then
		Leadership.Percent_NCO = 0.3
	elseif officer_ratio  < 1.2 then
		Leadership.Percent_NCO = 0.2
	elseif officer_ratio  < 1.3 then
		Leadership.Percent_NCO = 0.1
	
	-- Check to see if you have to many officers
	--    if so increase research
	elseif officer_ratio > 1.39 then
		Leadership.Percent_NCO = 0.0
	end
	
	-- If the AI has to many diplomats then set it to 0 (100 is max you can have)
	-- If the NCO desperation is true try and shift diplomacy into NCO production instead of Research
	if StandardDataObject.IsMajor then
		if Leadership.NCONeeded then
			Leadership.Percent_NCO = Leadership.Percent_NCO + Leadership.Percent_Diplomacy
			Leadership.Percent_Diplomacy = 0
		else
			if Leadership.Diplomats > 50 then
				-- Make it so they have exactly what they need to maintain the influence
				if Leadership.ActiveInfluence > 0 then
					Leadership.Percent_Diplomacy = Leadership.ActiveInfluence / Leadership.TotalLeadership
				else
					Leadership.Percent_Diplomacy = 0
				end
			end
		end
	else
		if Leadership.NCONeeded then
			Leadership.Percent_NCO = Leadership.Percent_NCO + Leadership.Percent_Diplomacy
			Leadership.Percent_Diplomacy = 0
		elseif Leadership.Diplomats > 20 then
			Leadership.Percent_Diplomacy = 0
		elseif Leadership.Diplomats > 15 then
			Leadership.Percent_Diplomacy = Leadership.Percent_Diplomacy / 4
		elseif Leadership.Diplomats > 10 then
			Leadership.Percent_Diplomacy = Leadership.Percent_Diplomacy / 2
		end	
	end
	
	-- Apply the diplomacy caps
    if Leadership.Percent_Diplomacy > 0 then
		if StandardDataObject.IsMajor then
			if Leadership.CanInfluence then
				Leadership.Percent_Diplomacy = (math.min(liDiplomacyInFaction, (Leadership.TotalLeadership * Leadership.Percent_Diplomacy)) / Leadership.TotalLeadership)
			else
				Leadership.Percent_Diplomacy = (math.min(liDiplomacyNoFaction, (Leadership.TotalLeadership * Leadership.Percent_Diplomacy)) / Leadership.TotalLeadership)
			end
		else
			Leadership.Percent_Diplomacy = (math.min(liDiplomacyNoFaction, (Leadership.TotalLeadership * Leadership.Percent_Diplomacy)) / Leadership.TotalLeadership)
		end
	end
	
	-- Research is whatever is left over
	Leadership.Percent_Research = (((1 - Leadership.Percent_Espionage) - Leadership.Percent_Diplomacy) - Leadership.Percent_NCO)
	
	Leadership.Slots_Research = Leadership.TotalLeadership * Leadership.Percent_Research
	Leadership.Slots_Espionage = Leadership.TotalLeadership * Leadership.Percent_Espionage
	Leadership.Slots_Diplomacy = Leadership.TotalLeadership * Leadership.Percent_Diplomacy
	Leadership.Slots_NCO = Leadership.TotalLeadership * Leadership.Percent_NCO
	
	-- Do not post unless set to true as this could be a call from other AIs to get information on the sliders
	if vbSliders then
		local command = CChangeLeadershipCommand(StandardDataObject.ministerTag, Leadership.Percent_NCO, Leadership.Percent_Diplomacy, Leadership.Percent_Espionage, Leadership.Percent_Research)
		StandardDataObject.ministerAI:Post(command)
	end
	
	return Leadership
end

-- Processes the main tech reasearch for the specified country
--   designed to be a recursive call in case the AI needs to research in the future
function Process_Tech(pYear, pMaxYear, ResearchSlotsAllowed, ResearchSlotsNeeded)
	-- Performance check, exit if there are no slots available
	if ResearchSlotsNeeded < 0.01 then
		return
	end
	if pYear >= pMaxYear then
		return
	end
	
	local lbIndustry = TechnologyData.TechStatus:IsBuildingAvailable(CBuildingDataBase.GetBuilding("industry"))
	local lbNavalBase = TechnologyData.TechStatus:IsBuildingAvailable(CBuildingDataBase.GetBuilding("naval_base"))
	local lbAirBase = TechnologyData.TechStatus:IsBuildingAvailable(CBuildingDataBase.GetBuilding("air_base"))
	local loInfra = {lbInfra = TechnologyData.TechStatus:IsBuildingAvailable(CBuildingDataBase.GetBuilding("infra")),
					liIndProdLevel = TechnologyData.TechStatus:GetLevel(CTechnologyDataBase.GetTechnology("industral_production")),
					liIndEffLevel = TechnologyData.TechStatus:GetLevel(CTechnologyDataBase.GetTechnology("industral_efficiency"))}
	
	
	-- Figure out if they need to research Infrastructure tech
	---   Flag set to true to fake the code to think they have it already
	--     if requirements are not met
	if not(loInfra.lbInfra) and pYear < 1938 then
		-- These two techs need to be level 3 or greater before they can research Infrastructure
		--    if we already have level three set the flag to true till 1938
		if loInfra.liIndProdLevel >= 3 and loInfra.liIndEffLevel >= 3 then
			lbInfra = true
		end
	end	
	
	--Utils.LUA_DEBUGOUT("Country: " .. tostring(ministerTag))
		
	local laPrimeTechAreas = {}
	laPrimeTechAreas[_RESEARCH_LAND_] = {Folder = {"infantry_folder",
										  "armour_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "LandTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_LAND_DOC_] = {Folder = {"land_doctrine_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "LandDoctrinesTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_AIR_] = {Folder = {"fighter_folder",
										 "bomber_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "AirTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_AIR_DOC_] = {Folder = {"air_doctrine_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "AirDoctrineTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_NAVAL_] = {Folder = {"smallship_folder",
										   "capitalship_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "NavalTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_NAVAL_DOC_] = {Folder = {"naval_doctrine_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "NavalDoctrineTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_INDUSTRIAL_] = {Folder = {"industry_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "IndustrialTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_SECRET_] = {Folder = {"secretweapon_folder"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "SecretWeaponTechs",
								ListIgnore = {},
								ListPrefer = {}}
	laPrimeTechAreas[_RESEARCH_UNKNOWN_] = {Folder = {"unknown"},
								ResearchWeight = 0, 
								CurrentSlots = 0, 
								ResearchSlots = 0, 
								RegularTech = {}, 
								PreferTech = {},
								ListName = "OtherTechs",
								ListIgnore = {},
								ListPrefer = {}}
	
	-- Prio research Industry and Infra above everything else
	if not(lbIndustry) or not(loInfra.lbInfra) then
		laPrimeTechAreas[_RESEARCH_LAND_].ResearchWeight = 0.25
		laPrimeTechAreas[_RESEARCH_LAND_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_AIR_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_AIR_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_NAVAL_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_NAVAL_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_INDUSTRIAL_].ResearchWeight = 0.75
		laPrimeTechAreas[_RESEARCH_SECRET_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_UNKNOWN_].ResearchWeight = 0.0
		
	elseif not(lbAirBase) then
		laPrimeTechAreas[_RESEARCH_LAND_].ResearchWeight = 0.5
		laPrimeTechAreas[_RESEARCH_LAND_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_AIR_].ResearchWeight = 0.5
		laPrimeTechAreas[_RESEARCH_AIR_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_NAVAL_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_NAVAL_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_INDUSTRIAL_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_SECRET_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_UNKNOWN_].ResearchWeight = 0.0
	
	elseif not(lbNavalBase) and TechnologyData.PortsTotal > 0 then
		laPrimeTechAreas[_RESEARCH_LAND_].ResearchWeight = 0.5
		laPrimeTechAreas[_RESEARCH_LAND_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_AIR_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_AIR_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_NAVAL_].ResearchWeight = 0.5
		laPrimeTechAreas[_RESEARCH_NAVAL_DOC_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_INDUSTRIAL_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_SECRET_].ResearchWeight = 0.0
		laPrimeTechAreas[_RESEARCH_UNKNOWN_].ResearchWeight = 0.0

	-- Country has the basics so check to see if it has country specific weights
	--    does the country have country specific weights
	else
		local laTechWeights
		local countryResearch = TechnologyData.ministerCountry:GetTotalLeadership():Get() --research
		
		--TODO RANDOMHOI checks if we need more resources to research resource-related industry
		
		if (countryResearch > 60) then --research ALL 
				laTechWeights = Utils.CallCountryAI("DEFAULT_ALL", "TechWeights", TechnologyData)	
		else
			
			if Utils.HasCountryAIFunction(TechnologyData.ministerTag, "TechWeights") then
				laTechWeights = Utils.CallCountryAI(TechnologyData.ministerTag, "TechWeights", TechnologyData)
			elseif TechnologyData.IsNaval then
				laTechWeights = Utils.CallCountryAI("DEFAULT_MIXED", "TechWeights", TechnologyData)
			else
				laTechWeights = Utils.CallCountryAI("DEFAULT_LAND", "TechWeights", TechnologyData)
			end
			
			for i = 1, _RESEARCH_UNKNOWN_ do
				laPrimeTechAreas[i].ResearchWeight = laTechWeights[i]
			end
		end
	end	
	
	
	
	-- Figure out what the AI currently is researching
	for tech in TechnologyData.ministerCountry:GetCurrentResearch() do
		local lbFound = false
		local lsFolder = tostring(tech:GetFolder():GetKey())
		
		for i = 1, (_RESEARCH_UNKNOWN_ - 1) do
			local subLength = table.getn(laPrimeTechAreas[i].Folder)
			
			for subI = 1, subLength do
				-- If Tech folder found now exit both loops
				if lsFolder == laPrimeTechAreas[i].Folder[subI] then
					laPrimeTechAreas[i].CurrentSlots = laPrimeTechAreas[i].CurrentSlots + 1	
					subI = subLength
					i = _RESEARCH_UNKNOWN_
					lbFound = true
				end
			end
		end

		-- It is Uknown so process it special
	    if lbFound == false then
			laPrimeTechAreas[_RESEARCH_UNKNOWN_].CurrentSlots = laPrimeTechAreas[_RESEARCH_UNKNOWN_].CurrentSlots + 1	
		end
	end
	
	for k, v in pairs(laPrimeTechAreas) do
		-- Retrieve Tech Ignore and Prefer Lists
		if Utils.HasCountryAIFunction(TechnologyData.ministerTag, v.ListName) then
			v.ListIgnore, v.ListPrefer =  Utils.CallCountryAI(TechnologyData.ministerTag, v.ListName, TechnologyData)
		else
			-- If their tech weights are land based and not country specific
			if TechnologyData.IsNaval then
				v.ListIgnore, v.ListPrefer =  Utils.CallCountryAI("DEFAULT_MIXED", v.ListName, TechnologyData)
			else
				v.ListIgnore, v.ListPrefer =  Utils.CallCountryAI("DEFAULT_LAND", v.ListName, TechnologyData)
			end
		end

		-- Overide to make sure they research industry, port, air-base and infra techs
		--   this will fire regardless of what the defaults are in the country specific LUA
		if v.ListName == "IndustrialTechs" then
			if not(lbIndustry) or not(lbInfra) then
				-- Reset the Array to make sure its empty
				v.ListPrefer = {}
				
				-- Make the AI focus on Factory building techs first
				if not(lbIndustry) then
					table.insert(v.ListPrefer, "construction_engineering")
				elseif not(loInfra.lbInfra) then
					table.insert(v.ListPrefer, "advanced_construction_engineering")
					
					if loInfra.liIndProdLevel < 3 then
						table.insert(v.ListPrefer, "industral_production")
					end
					if loInfra.liIndEffLevel < 3 then
						table.insert(v.ListPrefer, "industral_efficiency")
					end
				end
			end
			
		-- Overide to make sure they research Naval Bases
		elseif v.ListName == "NavalTechs" then
			if not(lbNavalBase) and TechnologyData.PortsTotal > 0 then
				v.ListPrefer = {"destroyer_technology"}
			end
			
		-- Overide to make sure they research Air Bases
		elseif v.ListName == "AirTechs" then
			if not(lbAirBase) then
				v.ListPrefer = {"single_engine_aircraft_design"}
			end
		end
		
		-- Calculate what the AI wants to research in each category based on the weights
		---  AI may put more slots in that it can research but thats no big deal
		v.ResearchSlots = math.max(0, Utils.Round((ResearchSlotsAllowed * v.ResearchWeight) - v.CurrentSlots))
	end
	
	-- Figure out what the AI can research
	for tech in CTechnologyDataBase.GetTechnologies() do
		if  TechnologyData.minister:CanResearch(tech) and tech:IsValid() then
			local nYear = TechnologyData.TechStatus:GetYear(tech, (TechnologyData.TechStatus:GetLevel(tech) + 1))
			
			-- Concentrate only on techs for the year requested or less
			--- Penalties are way to high to go into the future
			if nYear <= pYear then
				local liPrimeIndex = _RESEARCH_UNKNOWN_
				local lsFolder = tostring(tech:GetFolder():GetKey())
				
				for i = 1, (_RESEARCH_UNKNOWN_ - 1) do
					local subLength = table.getn(laPrimeTechAreas[i].Folder)
					
					for subI = 1, subLength do
						-- If Tech folder found now exit both loops
						if lsFolder == laPrimeTechAreas[i].Folder[subI] then
							subI = subLength
							liPrimeIndex = i
							i = _RESEARCH_UNKNOWN_
						end
					end
				end	

				local lsTechName = tostring(tech:GetKey())
				local lsTechLevel = TechnologyData.TechStatus:GetLevel(tech)

				-- Fill up the research arrays
				if TechIgnore(lsTechLevel, lsTechName, laPrimeTechAreas[liPrimeIndex].ListIgnore) == false then
					if TechPrefer(lsTechName, laPrimeTechAreas[liPrimeIndex].ListPrefer) == false then
						table.insert(laPrimeTechAreas[liPrimeIndex].RegularTech, tech )
					else
						table.insert(laPrimeTechAreas[liPrimeIndex].PreferTech, tech )
					end
				end
			end
		end
	end
	
	-- Holds extra research slots that the AI is unable to use
	local liExtraSlots = ResearchSlotsNeeded

	for k, v in pairs(laPrimeTechAreas) do
		-- Calculate to see if we are going to have extra research slots left over
		liExtraSlots = liExtraSlots - v.ResearchSlots
		
		-- Perform the research and recapture the returning object
		v = ResearchTech(false, v)
		
		-- Recalculate now because it the ResearchSlots tells you how many
		--    have not been used so you need to re-add them into the ExtraSlots
		liExtraSlots = liExtraSlots + v.ResearchSlots
	end
	
	if liExtraSlots > 0 then
		for k, v in pairs(laPrimeTechAreas) do
			-- Use the RsearchSlots parm to control how many to research
			v.ResearchSlots = liExtraSlots
			
			-- Perform the research and recapture the returning object
			--   stick to prefer techs first
			v = ResearchTech(true, v)

			-- Grab the extra slots for the next set
			liExtraSlots = v.ResearchSlots
		end
		
		if liExtraSlots > 0 then
			for k, v in pairs(laPrimeTechAreas) do
				-- Use the RsearchSlots parm to control how many to research
				v.ResearchSlots = liExtraSlots
				
				-- Perform the research and recapture the returning object
				v = ResearchTech(false, v)

				-- Grab the extra slots for the next set
				liExtraSlots = v.ResearchSlots
			end
			
			-- There are still slots so jump into future techs
			if liExtraSlots > 0 then
				-- We have extra slots and no techs to research so go ahead and look into the future.
				Process_Tech((pYear + 1), pMaxYear, ResearchSlotsAllowed, liExtraSlots)
			end
		end
	end
end

-- Decide if the tech is to be ignored or not
function TechIgnore(viTechLevel, vsTechName, vaIgnoreTechs)
	local lbIgnoreTech = false
	
	if (vaIgnoreTechs == nil) then
		return lbIgnoreTech
	end
	
	local i = 1
	local TableLength = table.getn(vaIgnoreTechs)
	
	-- Performance check
	if TableLength > 0 then
		-- Ignores every tech in teh category if set to "all"
		if vaIgnoreTechs[1] == "all" then
			lbIgnoreTech = true
		else
			-- Loop through the ignore list see if the tech is on it
			while i <= TableLength do
				if vsTechName == vaIgnoreTechs[i][1] then
					local TechLevel = vaIgnoreTechs[i][2]
					
					-- If the tech is the level specified or has it been marked for all levels
					---   then ignore it
					if viTechLevel >= TechLevel or TechLevel == 0 then
						lbIgnoreTech = true
						i = TableLength
					end
				end
				
				i = i + 1
			end						
		end
	end
	
	return lbIgnoreTech
end

-- Check to see if the tech is on the prefer list
--   The number being returned is used to tell it which array to place it in
function TechPrefer(vsTechName, vaPreferTechs)
	-- 0 = normal research tech
	-- 1 = prefered research tech
	local lbPreferTech = false

	-- Performance check, if nil get out
	if not(vaPreferTechs == nil) then
		local i = 1
		local TableLength = table.getn(vaPreferTechs)
		
		-- Performance check
		if TableLength > 0 then
			-- Loop through the ignore list see if the tech is on it
			while i <= TableLength do
				-- Prefer Research tech now get out of the loop
				if vsTechName == vaPreferTechs[i] then
					lbPreferTech = true
					i = TableLength
				end
				i = i + 1
			end						
		end
	end

	return lbPreferTech
end

-- Select a random tech from the array
function ResearchTech(vbPrioTechOnly, vaTechObject)
	-- Performance check make sure there is something to do
	if vaTechObject.ResearchSlots > 0 then
		local liNonPreferCount = table.getn(vaTechObject.RegularTech)
		local liPreferCount = 0
		
		-- Make sure there is a prefered tech option to process
		if not(vaTechObject.PreferTech == nil) then
			liPreferCount = table.getn(vaTechObject.PreferTech)
		end
		
		-- Ok first check now make sure one of the two main arrays has something
		if (liNonPreferCount > 0) or (liPreferCount > 0) then
			-- Normalize the max count for the loop as the request amount of techs can exceed what it has
			local liMainCount = math.min(vaTechObject.ResearchSlots, liPreferCount)
			
			-- Subtract what you are about to process
			vaTechObject.ResearchSlots = vaTechObject.ResearchSlots - liMainCount

			-- First process the Prefer techs
			local i = 0
			while i < liMainCount do
				local liTechSelected = math.random(liPreferCount - i)
				TechnologyData.ministerAI:Post(CStartResearchCommand(TechnologyData.ministerTag, vaTechObject.PreferTech[liTechSelected]))

				-- Remove the tech from the array
				table.remove(vaTechObject.PreferTech, liTechSelected)
				i = i + 1
			end
			
			-- If the vbPrioTechOnly is set to true then only process priority
			if vbPrioTechOnly == false then
				-- Now process the non-porefered techs
				--    normalize the loop count variable
				liMainCount = math.min(vaTechObject.ResearchSlots, liNonPreferCount)

				-- Subtract what you are about to process
				vaTechObject.ResearchSlots = vaTechObject.ResearchSlots - liMainCount
				
				i = 0
				while i < liMainCount do
					local liTechSelected = math.random(liNonPreferCount - i)
					TechnologyData.ministerAI:Post(CStartResearchCommand(TechnologyData.ministerTag, vaTechObject.RegularTech[liTechSelected]))
					-- Remove the tech from the array
					table.remove(vaTechObject.RegularTech, liTechSelected)
					
					i = i + 1
				end
			end
		end
	end
	
	return vaTechObject
end
