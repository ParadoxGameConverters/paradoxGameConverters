local P = {}
Support = P
-- ###############################################
-- Generic Support Methods
-- ###############################################

function P.IsFriendDistance(voAI, voFaction, voCountry, viAlignment)
	local liLowestScore = nil

	if viAlignment == nil then
		viAlignment = 100
	end
	
	if not(voCountry:HasFaction()) then
		for loFaction in CCurrentGameState.GetFactions() do
			if not(loFaction == voFaction) then
				-- They are aligning with another faction
				local liAlignment = math.floor(voAI:GetCountryAlignmentDistance(voCountry, loFaction:GetFactionLeader():GetCountry()):Get())
				
				if liAlignment < viAlignment then
					if not(liLowestScore) then
						liLowestScore = liAlignment
					elseif liLowestScore > liAlignment then
						liLowestScore = liAlignment
					end
				end
			end
		end
		
		if liLowestScore then
			return liLowestScore
		end
	end
	
	return 0
end

function P.IsFriend(voAI, voFaction, voCountry, viAlignment)
	if viAlignment == nil then
		viAlignment = 100
	end
	
	if not(voCountry:HasFaction()) then
		for loFaction in CCurrentGameState.GetFactions() do
			if not(loFaction == voFaction) then
				-- They are aligning with another faction
				local liAlignment = math.floor(voAI:GetCountryAlignmentDistance(voCountry, loFaction:GetFactionLeader():GetCountry()):Get())
				
				if liAlignment < viAlignment then
					return false
				end
			end
		end
	else
		if voCountry:GetFaction() ~= voFaction then
			return false
		end
	end
	
	return true
end

function P.CalculateExpenseResourceFactor(voCountry)
	local loEnergy = CResourceValues()
	local loMetal = CResourceValues()
	local loRare = CResourceValues()
	local loOil = CResourceValues()
	
	loEnergy:GetResourceValues( voCountry, CGoodsPool._ENERGY_ )
	loMetal:GetResourceValues( voCountry, CGoodsPool._METAL_ )
	loRare:GetResourceValues( voCountry, CGoodsPool._RARE_MATERIALS_ )
	loOil:GetResourceValues( voCountry, CGoodsPool._CRUDE_OIL_ )
	
	local liExpenseFactor = loEnergy.vDailyExpense * 0.5
	liExpenseFactor = liExpenseFactor + loMetal.vDailyExpense
	liExpenseFactor = liExpenseFactor + (loRare.vDailyExpense * 2)
	
	local liHomeFactor = Support.CalculateHomeProduced(loEnergy) * 0.5
	liHomeFactor = liHomeFactor + Support.CalculateHomeProduced(loMetal)
	liHomeFactor = liHomeFactor + (Support.CalculateHomeProduced(loRare) * 2)
	liHomeFactor = liHomeFactor + (Support.CalculateHomeProduced(loOil) * 3)
	
	return liExpenseFactor, liHomeFactor
end

function P.CalculateHomeProduced(loResource)
	local liDailyHome = loResource.vDailyHome
	
	if loResource.vConvoyedIn > 0 then
		-- If the Convoy in exceeds Home Produced by 10% it means they have a glutten coming in or
		--   are a sea bearing country like ENG or JAP
		--   so go ahead and count this as home produced up to 90% of it just in case something happens!
		if liDailyHome > loResource.vDailyExpense then
			liDailyHome = liDailyHome + loResource.vConvoyedIn
		else
			liDailyHome = liDailyHome + (loResource.vConvoyedIn * 0.9)
		end
	end	
	
	return liDailyHome
end

function P.GoodToWarCheck(voTargetTag, voTargetCountry, voForeignMinisterData, vbNeighborCheck, vbThreatCheck, vbFactionWarCheck, vbNapCheck)
	local lbDOW = false

	if voTargetCountry:Exists() then
		local loRelation = voForeignMinisterData.ministerCountry:GetRelation(voTargetTag)
	
		-- Do we have a Non Aggression Pact
		if not(loRelation:HasNap()) or vbNapCheck then
			-- Make Sure we are not already at war
			if not(loRelation:HasWar()) then
				-- Make sure they are our neighbor or Neighbor Check is bypassed
				local lbIsNeighbor = vbNeighborCheck
				
				if not(lbIsNeighbor) then
					lbIsNeighbor = voForeignMinisterData.ministerCountry:IsNonExileNeighbour(voTargetTag)
				end
				
				if lbIsNeighbor then
					-- If they have a faction check to see if we are already atwar with that faciton leader
					if voTargetCountry:HasFaction() then
						local loFaction = voTargetCountry:GetFaction()
						-- They are not in our faction
						if loFaction ~= voForeignMinisterData.Faction then
							if vbFactionWarCheck then
								lbDOW = true
							else
								local loFactionLeaderTag = voTargetCountry:GetFaction():GetFactionLeader()
								local loFactionRelation = voForeignMinisterData.ministerCountry:GetRelation(loFactionLeaderTag)
				
								-- If we are at war with that faction already then DOW
								if loFactionRelation:HasWar() then
									lbDOW = true
								end
							end
						end
					else
						-- check so we have threat or we shouldnt be randomly attacking neutrals
						if vbThreatCheck and (voForeignMinisterData.FactionName == "allies") then
							if loRelation:GetThreat():Get() > 30 then
								lbDOW = true
							end
						else
							lbDOW = true
						end
					end
				end
			end
		end
	end
	
	return lbDOW
end

function P.GoodToInfluence(voTargetTag, voTargetCountry, voDiploStatus, voForeignMinisterData)
	local lbInfluence = false
	
	if voTargetTag ~= voForeignMinisterData.ministerTag then
		if voTargetCountry:Exists() then
			if voTargetTag:IsReal() then
				if voTargetTag:IsValid() then
					if not(voTargetCountry:IsGovernmentInExile()) then
						if not(voTargetCountry:IsPuppet()) then
							if not(voForeignMinisterData.ministerCountry:HasDiplomatEnroute(voTargetTag)) then
								if not(voTargetCountry:HasFaction()) then
									if not(voDiploStatus:HasWar()) then
										lbInfluence = true
									end
								end
							end
						end
					end
				end
			end
		end
	end
	
	return lbInfluence
end

function P.IsAligning(voTargetTag, voFactionLeaderTag)
	local loCommand = CInfluenceAllianceLeader(voTargetTag, voFactionLeaderTag)
	
	-- Are they already aligning toward us check (true means no)
	if not(loCommand:IsSelectable()) then
		return true
	end										
	
	return false
end

function P.GoodToExecuteDecision(voDecision, voDecisions, voForeignMinisterData)
	if voDecisions[voDecision.Name] then
		if (voDecisions[voDecision.Name].Year >= voForeignMinisterData.Year
		and voDecisions[voDecision.Name].Month >= voForeignMinisterData.Month
		and voDecisions[voDecision.Name].Day >= voForeignMinisterData.Day)
		or
		(voDecisions[voDecision.Name].Year > voForeignMinisterData.Year) then
			if voDecisions[voDecision.Name].War then
				voForeignMinisterData.Strategy:PrepareWarDecision(CCountryDataBase.GetTag(voDecisions[voDecision.Name].Country), 100, voDecision.Decision, false)
			else
				return voDecisions[voDecision.Name].Score
			end
		end
	end
	
	return 0
end

function P.ExecuteCallAlly(vAI, voCallerTag, voAlly, voTargetTag)
	local liScore = DiploScore_CallAlly(vAI, voCallerTag, voAlly.AllyTag, voAlly.AllyTag, nil)
	if liScore > 50 then
		-- Call our Ally in
		local loCommand = CCallAllyAction( voCallerTag, voAlly.AllyTag, voTargetTag)
		loCommand:SetValue(true) -- limited
		
		if loCommand:IsSelectable() then
			vAI:PostAction(loCommand)
		end
	end
end

function P.ExecuteInfluence(vAI, voFromTag, voTargetTag, vbCancel)
	local loCommand = CInfluenceNation(voFromTag, voTargetTag)
	
	if vbCancel then
		loCommand:SetValue(false)
		vAI:PostAction(loCommand)
		return true
	elseif loCommand:IsSelectable() then
		vAI:PostAction(loCommand)
		return true
	end
	
	return false
end

function P.AlignmentNeutral(voForeignMinisterData)
	local loFactions = {}
	local lsFarthestFaction = nil
	local liFarthestFaction = nil
	local lbInfluence = false

	-- Look at where we are currently aligned to
	for k, v in pairs(voForeignMinisterData.IdeologyMaping) do
		local loLeaderTag = CCurrentGameState.GetFaction(voForeignMinisterData.IdeologyMaping[k]):GetFactionLeader()
		local loLeaderCountry = loLeaderTag:GetCountry()
		local liAlignment = voForeignMinisterData.ministerAI:GetCountryAlignmentDistance(voForeignMinisterData.ministerCountry, loLeaderCountry):Get()
		local loCommand = CInfluenceAllianceLeader(voForeignMinisterData.ministerTag, loLeaderTag)

		loFactions[k] = {
			Tag = loLeaderTag,
			Country = loLeaderCountry,
			Aligning = not(loCommand:IsSelectable()),
			Command = loCommand,
			Alignment = liAlignment}
			
		if not(lsFarthestFaction) then
			lsFarthestFaction = k
			liFarthestFaction = liAlignment
		elseif liAlignment > liFarthestFaction then
			lsFarthestFaction = k
			liFarthestFaction = liAlignment
		end
		
		if loFactions[k].Alignment < 180 then
			lbInfluence = true
		end
	end
	
	-- Now try and get us back into the middle
	for k, v in pairs(voForeignMinisterData.IdeologyMaping) do
		if k == lsFarthestFaction and lbInfluence then
			if loFactions[k].Command:IsSelectable() then
				voForeignMinisterData.ministerAI:PostAction(loFactions[k].Command)
			end
		else
			if not(loFactions[k].Command:IsSelectable()) then
				loFactions[k].Command:SetValue(false)
				if loFactions[k].Command:IsSelectable() then
					voForeignMinisterData.ministerAI:PostAction(loFactions[k].Command)
				end
			end
		end
	end
	
	return false
end

-- vbIdeology is an Overide for the Ideology check
-- vbIsFactionWar is an Overide for the faction we want to help being at war
function P.AlignmentPush(vsFaction, voForeignMinisterData, vbIdeology, vbIsFactionWar)
	if not(voForeignMinisterData.HasFaction) then
		local lsOurIdeology = tostring(voForeignMinisterData.ministerCountry:GetRulingIdeology():GetGroup():GetKey())
		
		if voForeignMinisterData.IdeologyMaping[lsOurIdeology] == vsFaction or vbIdeology then
			local loFactionLeaderTag = CCurrentGameState.GetFaction(vsFaction):GetFactionLeader()
			local loFactionCountry = loFactionLeaderTag:GetCountry()
			
			if loFactionCountry:IsAtWar() or vbIsFactionWar then
				local loCommand = CInfluenceAllianceLeader(voForeignMinisterData.ministerTag, loFactionLeaderTag)
				
				if loCommand:IsSelectable() then
					voForeignMinisterData.ministerAI:PostAction(loCommand)
				end
				
				-- Condition Met so turn off other pushes
				for loFaction in CCurrentGameState.GetFactions() do
					if tostring(loFaction:GetTag()) ~= vsFaction then
						loCommand = CInfluenceAllianceLeader(voForeignMinisterData.ministerTag, loFaction:GetFactionLeader())
						if not(loCommand:IsSelectable()) then
							loCommand:SetValue(false)
							if loCommand:IsSelectable() then
								voForeignMinisterData.ministerAI:PostAction(loCommand)
							end
						end
					end
					
				end
				
				return false
			end
		end
	end
	
	return true
end

-- ###############################################
-- End Generic Support Methods
-- ###############################################


-- ###############################################
-- Producton support methods
-- ###############################################
-- Creates the physical unit and places it in the production que
--	loType = {
--		Name = "infantry_brigade", 				### The unit type
--		Serial = 4, 							### How large of a serial run do you want
--		Size = 1,  								### How many of this unit type do you want in the division
--		Support = 2, 							### How many support units do you want
--		SupportGroup = "Infantry", 				### What support group to use
--		SecondaryMain = "bergsjaeger_brigade", 	### What secondary main unit do you want to attach to this (will attach 1 of them)
--		SubUnit = "cag", 						### Builds this is a seperate unit if the main one is built
--		SubQuantity = 1}  						### How many to build of the seperate unit

--  laSupportUnit = {
--		"anti_air_brigade",
--		"anti_tank_brigade"}

-- voType 			= look at the definition above (loType) on how to create the voType Array
-- vIC 				= how many IC is available to build this unit	
-- viUnitQuantity 	= How many of this unit type do we need to build
-- voProductionData	= Production Object from ai_production_minister.lua
-- vaSupportUnit	= (OPTIONAL) Look at the definition above (laSupportUnit) on how to create one.
-- vbForce			= (OPTIONAL) It will foce production of the unit and ignore IC cost (still checks MP)

function P.CreateUnit(voType, vIC, viUnitQuantity, voProductionData, vaSupportUnit, vbForce)
	if voType.Size == nil then voType.Size = 1 end
	
	-- Not enough to build the unit so exit	
	if voType.Size > viUnitQuantity then
		return vIC, voProductionData.ManpowerTotal, viUnitQuantity
	end
	
	local loUnitType = CSubUnitDataBase.GetSubUnit(voType.Name)
	if (vIC > 0.1 or vbForce) and voProductionData.TechStatus:IsUnitAvailable(loUnitType) then
		local loSecUnitType = nil
		local liSecManpowerCost = 0
		
		-- Secondary Main Unit Type setup
		if not(voType.SecondaryMain == nil) then
			loSecUnitType = CSubUnitDataBase.GetSubUnit(voType.SecondaryMain)
			
			-- If secondary main can not be built then exit out immediately
			if not(voProductionData.TechStatus:IsUnitAvailable(loSecUnitType)) then
				return vIC, voProductionData.ManpowerTotal, viUnitQuantity
			end
			
			liSecManpowerCost = loSecUnitType:GetBuildCostMP():Get()
		end
		
		-- Setup Parameter defaults
		if voType.Serial == nil then voType.Serial = 1 end
		if voType.Support == nil then voType.Support = 0 end
	
		local liUnitMPcost = loUnitType:GetBuildCostMP():Get()
		local liManpowerCostByUnit = ((liUnitMPcost * voType.Size) + liSecManpowerCost)
		local liManpowerLeft = voProductionData.ManpowerTotal
		
		if (liManpowerLeft > liManpowerCostByUnit) then
			local liSecUnitCost = 0
			local lbReserve = (not voProductionData.IsAtWar) and loUnitType:IsRegiment()

			-- Secondary Unit Cost Check
			if not(loSecUnitType == nil) then
				liSecUnitCost = voProductionData.ministerCountry:GetBuildCostIC( loSecUnitType, 1, lbReserve ):Get()
			end
		
			local liTotalDivisions = math.floor(viUnitQuantity / voType.Size)
			local i = 0 -- Counter for amount of units built
			local liUnitCost = (voProductionData.ministerCountry:GetBuildCostIC( loUnitType, 1, lbReserve ):Get()) + liSecUnitCost
			local laOriginalSupportArray = nil
				
			-- Performance reasons why its done outside of the loop
			if vaSupportUnit ~= nil then
				laOriginalSupportArray = Support.BuildCustomSupportArray(vaSupportUnit, voProductionData)
			elseif voType.SupportGroup ~= nil then
				laOriginalSupportArray = Support.BuildSupportArray(voType.SupportGroup, voProductionData)
			end
			
			while i < liTotalDivisions do
				local liBuildCount
				local laAttachUnitArray = nil
				
				-- If there is a support array then process it
				if laOriginalSupportArray ~= nil then
					laAttachUnitArray = {}
					
					-- Make a copy of the support table so it can be reused for each parrallel run
					for y = 1, table.getn(laOriginalSupportArray) do
						laAttachUnitArray[y] = laOriginalSupportArray[y]
					end
				end

				if 	liTotalDivisions >= (i + voType.Serial) then
					local liTManpowerCost = liManpowerCostByUnit * voType.Serial
					
					-- We have enough MP so continue
					if liManpowerLeft > liTManpowerCost then
						liBuildCount = voType.Serial
						i = i + voType.Serial
						liManpowerLeft = liManpowerLeft - liTManpowerCost
						
					-- We do not have enough MP so stick to what we can build
					else
						liBuildCount = math.floor(liManpowerLeft / liManpowerCostByUnit)
						i = liTotalDivisions
						liManpowerLeft = liManpowerLeft - (liManpowerCostByUnit * liBuildCount)
					end
				else
					local liTManpowerCost = liManpowerCostByUnit * liTotalDivisions
				
					if liManpowerLeft > liTManpowerCost then
						liBuildCount = liTotalDivisions - i
						liManpowerLeft = liManpowerLeft - liTManpowerCost
					else
						liBuildCount = math.floor(liManpowerLeft / liManpowerCostByUnit)
						liManpowerLeft = liManpowerLeft - (liManpowerCostByUnit * liBuildCount)
					end
					
					i = liTotalDivisions
				end
				
				vIC = vIC - (liUnitCost * voType.Size)

				local loBuildOrder = SubUnitList()
				-- Add the amount of brigades requested of the main type
				for m = 1, voType.Size, 1 do
					SubUnitList.Append( loBuildOrder, loUnitType )
				end

				-- Check to see if there is a secondary main
				if not(loSecUnitType == nil) then
					SubUnitList.Append( loBuildOrder, loSecUnitType )
				end
				
				-- Attach a minor brigade if one can be attached
				--   updated the vIC total for the minor unit being attached
				if laAttachUnitArray ~= nil then
					for x = 1, voType.Support do
						local liTotalSupportUnits = table.getn(laAttachUnitArray)

						if liTotalSupportUnits > 0 then
							local loSupportUnit = (math.random(liTotalSupportUnits))
							local liManpowerCostBySubUnit = laAttachUnitArray[loSupportUnit]:GetBuildCostMP():Get()
							
							-- Enough MP so attach the support unit
							if liManpowerLeft > liManpowerCostBySubUnit then
								SubUnitList.Append( loBuildOrder, laAttachUnitArray[loSupportUnit] )
								vIC = vIC - (voProductionData.ministerCountry:GetBuildCostIC( laAttachUnitArray[loSupportUnit], 1, lbReserve ):Get())
								liManpowerLeft = liManpowerLeft - liManpowerCostBySubUnit
							end
							
							-- Remove the unit from the array so it will not get 2 of the same support units
							table.remove(laAttachUnitArray, loSupportUnit)
							
						-- No Minors so exit out
						else
							x = voType.Support + 1
						end
					end
				end
					
				viUnitQuantity = viUnitQuantity - (voType.Size * liBuildCount)
				voProductionData.ministerAI:Post(CConstructUnitCommand(voProductionData.ministerTag, loBuildOrder, voProductionData.CapitalPrvID, liBuildCount, lbReserve, CNullTag(), CID()))

				-- Process sub unit (used mainly for Carriers to build CAG)
				if not(voType.SubUnit == nil) and liManpowerLeft > 0 then
					-- Call self
					local liUnitNeeds -- Variable is not used at all

					-- Copy the object so the original is not changed
					local loType = {Name = voType.SubUnit}
					for k, v in pairs(voProductionData.UnitTypes[voType.SubUnit]) do
						loType[k] = voProductionData.UnitTypes[voType.SubUnit][k]
					end
					
					loType.Serial = liBuildCount
					vIC, voProductionData.ManpowerTotal, liUnitNeeds = Support.CreateUnit(loType, vIC, (voType.SubQuantity * liBuildCount), voProductionData, nil, true)
				end
				
				-- Reset the ManpowerTotal
				voProductionData.ManpowerTotal = liManpowerLeft
				
				if vIC <= 0.1 and not(vbForce) then
					i = liTotalDivisions --Causes it to exit loop
				end
			end
		end
	end

	return vIC, voProductionData.ManpowerTotal, viUnitQuantity
end

-- Called by the CreateUnit method, it helps build a support unit array based on the SupportType
function P.BuildSupportArray(vsSupportGroup, voProductionData)
	local laUnitArray = {}
	
	for k, v in pairs(voProductionData.UnitTypes) do
		if v.SupportType ~= nil then
			if v.SupportType[vsSupportGroup] then 
				local loUnit = CSubUnitDataBase.GetSubUnit(k)
				if voProductionData.TechStatus:IsUnitAvailable(loUnit) then
					table.insert( laUnitArray, loUnit )
				end
			end
		end
	end
	
	return laUnitArray	
end

-- Called by the CreateUnit method, it helps build a custom support unit array
function P.BuildCustomSupportArray(vaSupportArray, voProductionData)
	local laUnitArray = {}
	
	for x = 1, table.getn(vaSupportArray) do
		local loUnit = CSubUnitDataBase.GetSubUnit(x)
		if voProductionData.TechStatus:IsUnitAvailable(loUnit) then
			table.insert( laUnitArray, loUnit )
		end
	end
	
	return laUnitArray	
end

-- #############################################
-- Building Type Construction Helper Methods
-- #############################################
function P.Build_RocketTest(...)
	return P.Build_Building("rocket_test", ...)
end

function P.Build_Radar(...)
	return P.Build_Building("radar_station", ...)
end

function P.Build_Fort(...)
	return P.Build_Building("land_fort", ...)
end
		
function P.Build_CoastalFort(...)
	return P.Build_Building("coastal_fort", ...)
end

function P.Build_AntiAir(...)
	return P.Build_Building("anti_air", ...)
end

function P.Build_Industry(...)
	return P.Build_Building("industry", ...)
end

function P.Build_Infrastructure(...)
	return P.Build_Building("infrastructure", ...)
end

function P.Build_AirBase(...)
	return P.Build_Building("air_base", ...)
end

function P.Build_NavalBase(...)
	return P.Build_Building("naval_base", ...)
end

function P.Build_NuclearReactor(...)
	return P.Build_Building("nuclear_reactor", ...)
end

-- Handles the creating of the building, this can be called directly
function P.Build_Building(vsBuildingType, vIC, voProductionData, viProvinceID, viMax)
	if vIC > 0.1 then
		local loProvince = CCurrentGameState.GetProvince(viProvinceID)
		local lbHasControl = (loProvince:GetController() == voProductionData.ministerTag)
	
		if lbHasControl then
			local loBuildingType = CBuildingDataBase.GetBuilding(vsBuildingType)
			local loBuilding = loProvince:GetBuilding(loBuildingType)

			if loBuilding:GetMax():Get() < viMax and loProvince:GetCurrentConstructionLevel(loBuildingType) == 0 then
				local loBuildingCost = voProductionData.ministerCountry:GetBuildCost(loBuildingType):Get()
				
				if vIC > 0.1 then
					local constructCommand = CConstructBuildingCommand(voProductionData.ministerTag, loBuildingType , viProvinceID, 1)
					
					if constructCommand:IsValid() then
						voProductionData.ministerAI:Post(constructCommand)
						vIC = vIC - loBuildingCost -- Update IC total
					end
				end
			end
		end
	end
	
	return vIC
end


-- ###############################################
-- END OF PRODUCTION methods
-- ###############################################

return Support