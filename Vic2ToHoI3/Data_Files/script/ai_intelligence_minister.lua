

local IntelligenceData = {} -- Gets reset each time the tick starts

-- Missions used in the PickBestMission (excludes special circumstances code)
local IntelligenceMissions = {
	AtPeace = {
		SpyMission.SPYMISSION_MILITARY,
		SpyMission.SPYMISSION_BOOST_OUR_PARTY,
		SpyMission.SPYMISSION_TECH
	},
	AtWar = {
		SpyMission.SPYMISSION_MILITARY,
		SpyMission.SPYMISSION_TECH,
		SpyMission.SPYMISSION_LOWER_NATIONAL_UNITY
	}
}

-- ###################################
-- # Main Method called by the EXE
-- #####################################
function IntelligenceMinister_Tick(minister)
	if math.mod( CCurrentGameState.GetAIRand(), 9) == 0 then
		-- Reset Global Array Container
		IntelligenceData = {
			minister = minister,
			ministerAI = minister:GetOwnerAI(),
			ministerTag = minister:GetCountryTag(),
			ministerCountry = nil,
			Faction = nil, -- Faction object for the country
			Capital = nil, -- Province object of the capital
			CapitalControlTag = nil, -- Tag of the country who controls the capital
			Continent = nil,
			humanTag = CCurrentGameState.GetPlayer(),
			Year = CCurrentGameState.GetCurrentDate():GetYear(),
			TotalDaysFromStart =  CCurrentGameState.GetCurrentDate():GetTotalDays(),
			IsAtWar = nil, -- Boolean are they atwar with someone
			IsMajor = nil, -- Boolean are they a major power
			IsExiled = nil, -- Boolean if they are a Goverment in Exile
			BufferUsedForMissions = 60, -- Buffer that is currently being used for missions
			DomesticBadSpies = false, -- Boolean indicating if there are bad spies present
			DomesticSpyPresense = nil,
			DomesticSpyPriority = nil,
			DomesticMission = nil,
			NationalUnity = nil,
			RullingIdeology = nil,
			RullingIdeologyGroup = nil,
			PartyPopularity = nil
		}	
	
		IntelligenceData.ministerCountry = IntelligenceData.ministerTag:GetCountry()
		IntelligenceData.Faction = IntelligenceData.ministerCountry:GetFaction()
		IntelligenceData.Capital = IntelligenceData.ministerCountry:GetCapitalLocation()
		IntelligenceData.CapitalControlTag = IntelligenceData.Capital:GetController()
		IntelligenceData.Continent = IntelligenceData.Capital:GetContinent()
		IntelligenceData.IsAtWar = IntelligenceData.ministerCountry:IsAtWar()
		IntelligenceData.IsMajor = IntelligenceData.ministerCountry:IsMajor()
		IntelligenceData.IsExiled = IntelligenceData.ministerCountry:IsGovernmentInExile()
		IntelligenceData.DomesticSpyPresense = IntelligenceData.ministerCountry:GetSpyPresence(IntelligenceData.ministerTag)
		IntelligenceData.DomesticSpyPriority = IntelligenceData.DomesticSpyPresense:GetPriority()
		IntelligenceData.DomesticMission = IntelligenceData.DomesticSpyPresense:GetPrimaryMission()
		IntelligenceData.NationalUnity = IntelligenceData.ministerCountry:GetNationalUnity():Get()
		IntelligenceData.RullingIdeology = IntelligenceData.ministerCountry:GetRulingIdeology()
		IntelligenceData.RullingIdeologyGroup = IntelligenceData.RullingIdeology:GetGroup()
		IntelligenceData.PartyPopularity = IntelligenceData.ministerCountry:AccessIdeologyPopularity():GetValue(IntelligenceData.RullingIdeology):Get()
	
		-- Are there bad spies in our country
		for loCountry in IntelligenceData.ministerCountry:GetSpyingOnUs() do
			local loSpyPresence = loCountry:GetCountry():GetSpyPresence(IntelligenceData.ministerTag)
			local loNewSpyMission = loSpyPresence:GetPrimaryMission()
			
			if loNewSpyMission == SpyMission.SPYMISSION_LOWER_NATIONAL_UNITY then
				IntelligenceData.DomesticBadSpies = true
				break
			end
		end

		ManageSpiesAtHome()
		ManageSpiesAbroad()
	end
end


function UpdateMissions(IntelligenceData, actorTag, targetTag, missions)
	local presence = actorTag:GetCountry():GetSpyPresence(targetTag)
	for miss, prio in pairs(missions) do
		if presence:GetMissionPriority(miss) ~= prio then
			IntelligenceData.ministerAI:Post(CChangeSpyMission(actorTag, targetTag, miss, prio))
		end
    end
end

function SetNoPriorities( missions )
	missions[SpyMission.SPYMISSION_COUNTER_ESPIONAGE] = 0
	missions[SpyMission.SPYMISSION_MILITARY]  = 0
	missions[SpyMission.SPYMISSION_TECH] = 0
	missions[SpyMission.SPYMISSION_BOOST_RULING_PARTY] = 0
	missions[SpyMission.SPYMISSION_BOOST_OUR_PARTY] = 0
	missions[SpyMission.SPYMISSION_LOWER_NATIONAL_UNITY] = 0
	missions[SpyMission.SPYMISSION_INCREASE_THREAT] = 0
	missions[SpyMission.SPYMISSION_RAISE_NATIONAL_UNITY] = 0
	missions[SpyMission.SPYMISSION_COVERT_OPS] = 0
end

function ManageSpiesAtHome()
	local liNewMission = nil
	
	local missions = {}
	SetNoPriorities( missions )
	
	-- Check to see if there are any special hooks
	if Utils.HasCountryAIFunction(IntelligenceData.ministerTag, "Intel_Home") then
		liNewMission = Utils.CallCountryAI(IntelligenceData.ministerTag, "Intel_Home", IntelligenceData)
	end
	
	if liNewMission == nil then

		missions[SpyMission.SPYMISSION_COUNTER_ESPIONAGE] = 3

		-- Raise our unity so we dont surrender so easy and can get special laws past
		if IntelligenceData.NationalUnity < 70 then
			missions[SpyMission.SPYMISSION_RAISE_NATIONAL_UNITY] = 2
		-- Counter Espionage check
		elseif IntelligenceData.DomesticBadSpies then 
			liNewMission = nil
		-- Support for our party is diminishing so raise it
		elseif IntelligenceData.PartyPopularity < 35 then
			missions[SpyMission.SPYMISSION_BOOST_RULING_PARTY] = 1
		-- Nothing really to do but our unity is not 90 so raise it
		elseif IntelligenceData.NationalUnity < 80 then
			missions[SpyMission.SPYMISSION_RAISE_NATIONAL_UNITY] = 1
		-- Support for our party to try and get rid of fractured government
		elseif IntelligenceData.PartyPopularity < 65 then
			missions[SpyMission.SPYMISSION_BOOST_RULING_PARTY] = 1
		-- Nothing really to do but our unity is not 90 so raise it
		elseif IntelligenceData.NationalUnity < 95 then
			missions[SpyMission.SPYMISSION_RAISE_NATIONAL_UNITY] = 1
		end
	else
		missions[liNewMission] = 3
	end
	
	-- Assign the mission
	UpdateMissions(IntelligenceData, IntelligenceData.ministerTag, IntelligenceData.ministerTag, missions)
	
	-- Always set your home priority to the highest
	if IntelligenceData.DomesticSpyPriority < CSpyPresence.MAX_SPY_PRIORITY then
		IntelligenceData.ministerAI:Post(CChangeSpyPriority(IntelligenceData.ministerTag, IntelligenceData.ministerTag, CSpyPresence.MAX_SPY_PRIORITY))
	end
end


function ManageSpiesAbroadAllies( loIntelCountry, missions )
	loIntelCountry.RullingIdeology = loIntelCountry.ministerCountry:GetRulingIdeology()
	loIntelCountry.RullingIdeologyGroup = loIntelCountry.RullingIdeology:GetGroup()
	loIntelCountry.PartyPopularity = loIntelCountry.ministerCountry:AccessIdeologyPopularity():GetValue(loIntelCountry.RullingIdeology):Get()
	
	local liNewPriority = 1
	if loIntelCountry.IsExiled then
		liNewPriority = 0
	end
							
	if liNewPriority > 0 then
		-- Default if we cant figure out what to do
		missions[SpyMission.SPYMISSION_COUNTER_ESPIONAGE] = 2
		
		-- If we are allies and they are of the same group support the rulling party
		if loIntelCountry.RullingIdeologyGroup == IntelligenceData.RullingIdeologyGroup then
			if loIntelCountry.PartyPopularity < 65 then
				missions[SpyMission.SPYMISSION_BOOST_RULING_PARTY] = 1
			end
		else
			-- We are allies but our ideologies are different make sure we are a major power and if so try and influence minors to our side
			if IntelligenceData.IsMajor then
				missions[SpyMission.SPYMISSION_BOOST_OUR_PARTY] = 1
			end
		end
	end

	return liNewPriority
end


function ManageSpiesAbroadNonAllies( loIntelCountry, missions )
	loIntelCountry.Relation = IntelligenceData.ministerCountry:GetRelation(loIntelCountry.ministerTag)
	loIntelCountry.AtWar = loIntelCountry.Relation:HasWar()
	loIntelCountry.IsNeighbor = IntelligenceData.ministerCountry:IsNonExileNeighbour(loIntelCountry.ministerTag)
	loIntelCountry.IsMajor = loIntelCountry.ministerCountry:IsMajor()
	loIntelCountry.Capital = loIntelCountry.ministerCountry:GetCapitalLocation()
	loIntelCountry.Continent = loIntelCountry.Capital:GetContinent()

	 -- Special neighbor check flag (uses IsFriend check)
	local lbIsFriend = Support.IsFriend(IntelligenceData.ministerAI, IntelligenceData.Faction, loIntelCountry.ministerCountry)
	local lbIsNeighbor = (loIntelCountry.IsNeighbor and not(lbIsFriend))
	
	-- Check to see if there are any special hooks
	local liNewPriority = 0
	if Utils.HasCountryAIFunction(IntelligenceData.ministerTag, "Intel_Priority") then
		liNewPriority = Utils.CallCountryAI(IntelligenceData.ministerTag, "Intel_Priority", IntelligenceData, loIntelCountry)				
	end	

	if not(loIntelCountry.IsExiled) then
		-- if its a neighbor always give them level 1 spy
		if lbIsNeighbor then
			liNewPriority = liNewPriority + 1
		end

		-- If its another major power and not a friend
		---   Mainly covers situations when USA invades France then Germany will increase 
		---   it spies priority in the USA from 1 to a 2 as they would be neighbors then.
		if IntelligenceData.IsMajor and loIntelCountry.IsMajor and not(lbIsFriend) then
			liNewPriority = liNewPriority + 1
		end
		
		-- if they are on the same continent proceed
		if IntelligenceData.Continent == loIntelCountry.Continent then
			-- If they are not a friend add another weight
			if not lbIsFriend then
				liNewPriority = liNewPriority + 1
			end
			
			-- If we are atwar with them
			if loIntelCountry.AtWar then
				liNewPriority = liNewPriority + 1
			end
		end
	end

	-- Check to see if there are any special hooks
	if Utils.HasCountryAIFunction(IntelligenceData.ministerTag, "Intel_Mission") then
		local loNewSpyMission = Utils.CallCountryAI(IntelligenceData.ministerTag, "Intel_Mission", IntelligenceData, loIntelCountry)
		if loNewSpyMission ~= nil then
			missions[loNewSpyMission] = 3
		end
	end
	
	-- If there is no weight set then we do not want to disrupt relations with them
	if liNewPriority > 0 then
		loNewSpyMission = PickBestMissions(loIntelCountry, lbIsNeighbor, missions)
	end

	return liNewPriority
end


function ManageSpiesAbroad()
	for loTCountry in CCurrentGameState.GetCountries() do
		if loTCountry:Exists() then
			-- Make sure its not the same country
			local loTargetTag = loTCountry:GetCountryTag()
		
			if loTargetTag ~= IntelligenceData.ministerTag then

				local missions = {}
				SetNoPriorities( missions )

				-- Create the Countries Object (in case there are hooks)
				local loIntelCountry = {
					ministerCountry = loTCountry,
					ministerTag = loTargetTag,
					SpyPresence = IntelligenceData.ministerCountry:GetSpyPresence(loTargetTag),
					CurrentMission = nil, -- Current Mission assigned to the country
					CurrentPriority = nil,-- Current Priority assigned to the country
					IsAlly = false, -- Are they an ally
					IsExiled = false, -- Boolean if they are a Goverment in Exile
					
					-- Following are only available if they are not allies
					Relation = nil, -- Relation object between the two countries
					AtWar = nil, -- Are the two countries at war with eachother
					IsNeighbor = nil, -- Are the two countries neighbors
					IsMajor = nil, -- Are they a major power
					Capital = nil, -- Province Object for their Capital
					Continent = nil, -- Continent the Capital is on
					
					-- Following are available if they are allies
					RullingIdeology = nil, -- Rulling Ideology
					RullingIdeologyGroup= nil, -- Idelogy group of the rulling government
					PartyPopularity = nil -- Popularity of the Rulling Ideology
				}
				
				loIntelCountry.CurrentMission = loIntelCountry.SpyPresence:GetPrimaryMission()
				loIntelCountry.IsExiled = loIntelCountry.ministerCountry:IsGovernmentInExile()
				loIntelCountry.IsAlly = IntelligenceData.ministerCountry:CalculateIsAllied(loIntelCountry.ministerTag)
				loIntelCountry.CurrentPriority = loIntelCountry.SpyPresence:GetPriority()
				
				local liNewPriority = 0
				if loIntelCountry.IsAlly then
					liNewPriority = ManageSpiesAbroadAllies( loIntelCountry, missions )
				else
					liNewPriority = ManageSpiesAbroadNonAllies( loIntelCountry, missions )
				end
				
				-- Normalize Priority
				liNewPriority = math.min( liNewPriority, CSpyPresence.MAX_SPY_PRIORITY )

				-- If prio changes then update it
				if loIntelCountry.CurrentPriority ~= liNewPriority then
					IntelligenceData.ministerAI:Post(CChangeSpyPriority(IntelligenceData.ministerTag, loIntelCountry.ministerTag, liNewPriority))
				end

				UpdateMissions(IntelligenceData, IntelligenceData.ministerTag, loIntelCountry.ministerTag, missions)				
			end
		end
	end
end

function PickBestMissions(voIntelCountry, vbIsNeighbor, missions)
	-- Goverment in Exile special check
	if IntelligenceData.IsExiled and (IntelligenceData.CapitalControlTag == voIntelCountry.ministerTag) then
		missions[SpyMission.SPYMISSION_MILITARY] = 3
	else	
		-- Are the two countries NOT at war
		if not voIntelCountry.AtWar then
			-- We are not at war with each other so only pick passive spy missions
			local nMissions = table.getn(IntelligenceMissions.AtWar)
			missions[ IntelligenceMissions.AtPeace[math.random(nMissions)] ] = 2
			missions[ IntelligenceMissions.AtPeace[math.random(nMissions)] ] = 1 -- ok if same, just means focus on one
		-- The two countries are at war
		else
			if loNewMission == nil then
				-- If we are neighbors and they are close to surrendering
				if vbIsNeighbor and (voIntelCountry.ministerCountry:GetSurrenderLevel():Get() > 0.6 ) then
					missions[SpyMission.SPYMISSION_LOWER_NATIONAL_UNITY] = 3				
				else -- else random pick 2
					local nMissions = table.getn(IntelligenceMissions.AtWar)
					missions[ IntelligenceMissions.AtWar[math.random(nMissions)] ] = 2
					missions[ IntelligenceMissions.AtWar[math.random(nMissions)] ] = 1 -- ok if same, just means focus on one
				end
			end
		end
	end
end

