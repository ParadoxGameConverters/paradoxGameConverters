--modificado para randomhoi

require('ai_diplomacy')

local ForeignMinisterData = {}

function Fill_ForeignMinisterData(minister, lbDataFilled)
	if not(lbDataFilled) then
		ForeignMinisterData = {
			minister = minister,
			ministerAI = minister:GetOwnerAI(),
			ministerTag = minister:GetCountryTag(),
			ministerCountry = nil,
			humanTag = CCurrentGameState.GetPlayer(),
			Year = CCurrentGameState.GetCurrentDate():GetYear(),
			Month = CCurrentGameState.GetCurrentDate():GetMonthOfYear(),
			Day = CCurrentGameState.GetCurrentDate():GetDayOfMonth(),
			Desperation = nil, 
			Strategy = nil, -- Strategy Object
			IsAtWar = nil, -- Boolean are they atwar with someone
			IsMajor = nil, -- Boolean are they a major power
			Faction = nil, -- Faction the country is in
			FactionName = nil, -- The name of the faction as a string
			HasFaction = nil} -- Boolean do they have a faction
		
		ForeignMinisterData.ministerCountry = ForeignMinisterData.ministerTag:GetCountry()
		ForeignMinisterData.IsAtWar = ForeignMinisterData.ministerCountry:IsAtWar()
		ForeignMinisterData.Strategy = ForeignMinisterData.ministerCountry:GetStrategy()
		ForeignMinisterData.IsMajor = ForeignMinisterData.ministerCountry:IsMajor()
		ForeignMinisterData.Faction = ForeignMinisterData.ministerCountry:GetFaction()
		ForeignMinisterData.FactionName = tostring(ForeignMinisterData.Faction:GetTag())
		ForeignMinisterData.HasFaction = ForeignMinisterData.ministerCountry:HasFaction()
		ForeignMinisterData.Desperation = ForeignMinisterData.ministerCountry:CalcDesperation():Get()
		ForeignMinisterData.IdeologyMaping = {fascism = "axis", democracy = "allies", communism = "comintern"}
	end
	
	return true
end

-- ###################################
-- # Called by the EXE
-- #####################################
function ForeignMinister_OnWar( agent, countryTag1, countryTag2, war )
	--if war:IsLimited() then
		-- dont pull anything else right now, lets wait until we need it
	--end
end

-- ###################################
-- # Called by the EXE
-- #####################################
function ForeignMinister_EvaluateDecision(minister, voDecisions, voScope) 
	local liScore = math.random(100)

	local loFunRef = Utils.HasCountryAIFunction(minister:GetCountryTag(), "ForeignMinister_EvaluateDecision")
	
	if loFunRef then
		local loDecision = {
			Name = tostring(voDecisions:GetKey()),
			Score = liScore,
			Decision = voDecisions}

		local lbDataFilled = Fill_ForeignMinisterData(minister, false)
		liScore = loFunRef(loDecision, ForeignMinisterData)
	end

	if liScore < 25 then
		liScore = 0
	end
	return liScore
end

-- ###################################
-- # Called by the EXE
-- #####################################
function ForeignMinister_Tick(minister)
	-- Execute Decisions
	minister:ExecuteDiploDecisions()
	
	local lbDataFilled = false
	
	-- Call Allies in for Help
	if math.random(6) == 1 then
		lbDataFilled = Fill_ForeignMinisterData(minister, lbDataFilled)
		ForeignMinister_CallAlly()
	end

	-- Ask for Military Access
	if math.random(16) == 1 then
		lbDataFilled = Fill_ForeignMinisterData(minister, lbDataFilled)
		ForeignMinister_MilitaryAccess()
	end
	


	-- Never Run Peace and Influence on same tick
	if math.random(16) == 1 then
		lbDataFilled = Fill_ForeignMinisterData(minister, lbDataFilled)		
		ForeignMinister_HandlePeace()
	elseif math.random(12) == 1 then
		lbDataFilled = Fill_ForeignMinisterData(minister, lbDataFilled)
		
		-- Only Major Powers perform this action
		if ForeignMinisterData.IsMajor and ForeignMinisterData.HasFaction then
			ForeignMinister_Influence()
		end
	end
	
	-- Alignment
	if math.random(12) == 1 then
		lbDataFilled = Fill_ForeignMinisterData(minister, lbDataFilled)		
		ForeignMinister_Alignment()
	end	
end



function ForeignMinister_MilitaryAccess()
	-- Exit there is nothing for us to do here
	if not(ForeignMinisterData.IsAtWar) then
		return
	end
	
	local lbProcess = Utils.CallFunction(ForeignMinisterData.ministerTag, "ForeignMinister_MilitaryAccess", ForeignMinisterData)

	-- Request for Military Access
	--  Only major powers will request military access
	if lbProcess then
		if ForeignMinisterData.IsMajor then
			for loCountryTag in ForeignMinisterData.ministerCountry:GetNeighbours() do
				local loCountry = loCountryTag:GetCountry()
				
				-- Do not bother asking major powers for military access
				if not(loCountry:IsMajor()) then
					-- If they are already in a faction do not bother them
					-- If they are in a war already do not bother them
					if not(loCountry:HasFaction()) and not(loCountry:IsAtWar()) then
						local loRelation = ForeignMinisterData.ministerAI:GetRelation(ForeignMinisterData.ministerTag, loCountryTag)

						-- Make sure we do not already have military access
						if not(loRelation:HasMilitaryAccess()) then
							local lbAsk = false
							
							-- Now check their neighbors to see if they touch an enemy
							for loCountryTag2 in loCountry:GetNeighbours() do
								if not(loCountryTag2 == ForeignMinisterData.ministerTag) then
									local loRelation2 = ForeignMinisterData.ministerAI:GetRelation(ForeignMinisterData.ministerTag, loCountryTag2)
								
									if loRelation2:HasWar() then
										lbAsk = true
										break
									end
								end
							end
							
							if lbAsk then
								if not(ForeignMinisterData.Strategy:IsPreparingWarWith(loCountryTag)) then
									local loAction = CMilitaryAccessAction(ForeignMinisterData.ministerTag, loCountryTag)

									if loAction:IsSelectable() then
										local liScore = DiploScore_DemandMilitaryAccess(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loCountryTag, ForeignMinisterData.ministerTag)

										if liScore > 50 then
											ForeignMinisterData.minister:Propose(loAction, liScore)
										end
									end
								end
							end
						end
					end
				end
			end
		end
	else
		lbProcess = true
	end
end

function ForeignMinister_CallAlly()
	-- Exit there is nothing for us to do here
	if not(ForeignMinisterData.IsAtWar) then
		return
	end

	-- Call our Allies in
	local lbProcess = Utils.CallFunction(ForeignMinisterData.ministerTag, "ForeignMinister_CallAlly", ForeignMinisterData)
	
	if lbProcess then
		-- Get a list of all your allies
		local laAllies = {}
		for loAllyTag in ForeignMinisterData.ministerCountry:GetAllies() do
			local loAllyCountry = loAllyTag:GetCountry()

			-- Exclude Puppets from this list /dont
			--if not(loAllyCountry:IsPuppet()) then
			local loAlly = {
				AllyTag = loAllyTag,
				AllyCountry = loAllyCountry
			}
				
			laAllies[tostring(loAllyTag)] = loAlly
			--end
		end
	
		for loDiploStatus in ForeignMinisterData.ministerCountry:GetDiplomacy() do
			local loTargetTag = loDiploStatus:GetTarget()
			
			if loTargetTag:IsValid() and loDiploStatus:HasWar() then
				local loWar = loDiploStatus:GetWar()
				
				if loWar:IsLimited() then
					-- do we want to call in help?
					if loWar:GetCurrentRunningTimeInMonths() > 5 then
						if ForeignMinisterData.ministerCountry:CalcDesperation():Get() > 0.4 then --strengthFactor < 1.4 then
							-- Call in all potential allies
							for k, v in pairs(laAllies) do
								if not (v.AllyCountry:IsPuppet()) and not (v.AllyCountry:GetRelation(loTargetTag):HasWar()) then
									Support.ExecuteCallAlly(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, v, loTargetTag)
								end
							end
						end
					end

					-- always call puppets
					for loPuppet in ForeignMinisterData.ministerCountry:GetVassals() do
						if not (loPuppet:GetCountry():GetRelation(loTargetTag):HasWar()) then
							local loPuppetAlly = {
								AllyTag = loPuppet,
								AllyCountry = loPuppet:GetCountry()
							}
							Support.ExecuteCallAlly(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loPuppetAlly, loTargetTag)
						end
					end

				else -- not-limited, call in any faction members not there:
					-- Call in all potential allies
					for k, v in pairs(laAllies) do
						if not(v.AllyCountry:GetRelation(loTargetTag):HasWar()) then
							Support.ExecuteCallAlly(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, v, loTargetTag)
						end
					end
				end
			end
		end
	end
end

function ForeignMinister_Alignment()
	if not(ForeignMinisterData.HasFaction) then
		local lbProcess = Utils.CallFunction(ForeignMinisterData.ministerTag, "ForeignMinister_Alignment", ForeignMinisterData)
	
		if lbProcess then
			local lsOurIdeology = tostring(ForeignMinisterData.ministerCountry:GetRulingIdeology():GetGroup():GetKey())
			local loLeaderTag = CCurrentGameState.GetFaction(ForeignMinisterData.IdeologyMaping[lsOurIdeology]):GetFactionLeader()
			local loLeaderCountry = loLeaderTag:GetCountry()
			local liAlignment = ForeignMinisterData.ministerAI:GetCountryAlignmentDistance(ForeignMinisterData.ministerCountry, loLeaderCountry):Get()
			
			local loCommand = CInfluenceAllianceLeader(ForeignMinisterData.ministerTag, loLeaderTag)
			
			-- We are to far from our normal alignment so drift back
			if loCommand:IsSelectable() and liAlignment > 150 then
				ForeignMinisterData.ministerAI:PostAction(loCommand)
			elseif liAlignment < 100 then
				-- To Cancel the Influence
				loCommand:SetValue(false)
				if loCommand:IsSelectable() then
					ForeignMinisterData.ministerAI:PostAction(loCommand)
				end
			end
		end
	end
end

function ForeignMinister_Influence()
	-- These two variables only used for major powers
	local liInfluenceLeft = 0
	local liInfluenceCost = defines.diplomacy.INFLUENCE_INFLUENCE_COST
	local loLeaderShip = BalanceLeadershipSliders(ForeignMinisterData, false) -- Located in the ai_tech_minister.lua file
	
	-- We are alowed to influence so figure out what we can do
	if loLeaderShip.CanInfluence then
		liInfluenceLeft = math.floor(((loLeaderShip.TotalLeadership * loLeaderShip.Default_Diplomacy) / 2) - loLeaderShip.ActiveInfluence)
		
	-- We can't influence and we have something turned on so lets push this down to a negative to get rid of it
	elseif loLeaderShip.ActiveInfluence > 0 then
		liInfluenceLeft = -1
	
	-- Performance: We can't influence anything so why process this
	elseif loLeaderShip.ActiveInfluence == 0 then
		return false
	end
	
	local laIgnoreWatch = {} -- Ignore this country but monitor them if they are about to join someone else
	local laWatch = {} -- Monitor them and also fi their score is high enough they can be influenced normally
	local laIgnore = {} -- Ignore them completely	
	local loWatchCountry = nil
	local loBestCountry = nil
	local loCurrentlyInfluencing = {}
	local loFunRef = Utils.HasCountryAIFunction(ForeignMinisterData.ministerTag, "ForeignMinister_Influence")
	
	if loFunRef then
		laWatch, laIgnoreWatch, laIgnore = loFunRef(ForeignMinisterData)
		laWatch = Utils.Set(laWatch)
		laIgnoreWatch = Utils.Set(laIgnoreWatch)
		laIgnore = Utils.Set(laIgnore)
	end
	
	local loFactionLeaderTag = ForeignMinisterData.Faction:GetFactionLeader()
	
	for loRelation in ForeignMinisterData.ministerCountry:GetDiplomacy() do
		local loTargetTag = loRelation:GetTarget()
		local lsTargetTag = tostring(loTargetTag)
		
		-- Do not process if on the ignore list
		if not(laIgnore[lsTargetTag]) then
			local loTargetCountry = loTargetTag:GetCountry()

			-- If they are aligning do not influence
			if Support.GoodToInfluence(loTargetTag, loTargetCountry, loRelation, ForeignMinisterData) then
				-- If lbIsAligning = true and lbIsInfluencing = true : means we are influencing them
				-- If lbIsAligning = true and lbIsInfluencing = false : means they are aligning to us
				local lbIsAligning = Support.IsAligning(loTargetTag, loFactionLeaderTag)
				local lbIsNeighbor = ForeignMinisterData.ministerCountry:IsNonExileNeighbour(loTargetTag)
				
				if lbIsAligning then
					local loReverseRelation = loTargetCountry:GetRelation(ForeignMinisterData.ministerTag)
					local lbIsInfluencing = loReverseRelation:IsBeingInfluenced()
					
					if lbIsInfluencing then
						loCurrentlyInfluencing[lsTargetTag] = {
							Name = lsTargetTag,
							Tag = loTargetTag,
							Country = loTargetCountry,
							Distance = Support.IsFriendDistance(ForeignMinisterData.ministerAI, ForeignMinisterData.Faction, loTargetCountry),
							BeingWatched = (lbIsNeighbor or laWatch[lsTargetTag] or laIgnoreWatch[lsTargetTag]),
							Score = DiploScore_InfluenceNation(ForeignMinisterData, loTargetTag)}
					end
				else
					local loCountry = {
						Name = lsTargetTag,
						Tag = loTargetTag,
						Country = loTargetCountry,
						Distance = 0,
						BeingWatched = (lbIsNeighbor or laWatch[lsTargetTag] or laIgnoreWatch[lsTargetTag]),
						Score = DiploScore_InfluenceNation(ForeignMinisterData, loTargetTag)}
					
					-- Are they our neighbor or on our watch list
					if loCountry.BeingWatched then
						loCountry.Distance = Support.IsFriendDistance(ForeignMinisterData.ministerAI, ForeignMinisterData.Faction, loTargetCountry)
						
						if loCountry.Distance > 0 then
							if not(loWatchCountry) then
								loWatchCountry = loCountry
							elseif loCountry.Distance < loWatchCountry.Distance then
								loWatchCountry = loCountry
							end
							
						-- If they are on watch/ignore list do not process
						elseif not(laIgnoreWatch[lsTargetTag]) then
							if not(loBestCountry) then
								loBestCountry = loCountry
							elseif loCountry.Score > loBestCountry.Score then
								loBestCountry = loCountry
							end					
						end
					elseif not(loBestCountry) then
						loBestCountry = loCountry
					elseif loCountry.Score > loBestCountry.Score then
						loBestCountry = loCountry
					end
				end
			end
		else
			-- They are on the ignore list check to see if we are influencing them
			local loTargetCountry = loTargetTag:GetCountry()
			local loReverseRelation = loTargetCountry:GetRelation(ForeignMinisterData.ministerTag)
			local lbIsInfluencing = loReverseRelation:IsBeingInfluenced()
			
			if lbIsInfluencing then
				loCurrentlyInfluencing[lsTargetTag] = {
					Name = lsTargetTag,
					Tag = loTargetTag,
					Country = loTargetCountry,
					Distance = 0,
					BeingWatched = false,
					Score = 0}
			end
		end
	end
	
	-- Figure out who are our worst scorers
	local lbProcess = true
	local loWorstDistanceCountry = nil
	local loWorstCountry = nil
	local loSecondLowestCountry = nil
	
	for k, v in pairs(loCurrentlyInfluencing) do
		if v.Distance > 0 then
			if not(loWorstDistanceCountry) then
				loWorstDistanceCountry = v
			elseif v.Distance < loWorstDistanceCountry.Distance then
				loWorstDistanceCountry = v
			end
		end
		
		if not(loWorstCountry) then
			loWorstCountry = v
		elseif v.Score < loWorstCountry.Score then
			loSecondLowestCountry = loWorstCountry
			loWorstCountry = v
		end
	end
	
	-- If we have atleast one worst country to switch with
	if loWatchCountry 
	and liInfluenceLeft >= 0 
	and loLeaderShip.CanInfluence 
	and loLeaderShip.Diplomats >= liInfluenceCost then
		-- Check to see if they are already being influenced
		if not(loCurrentlyInfluencing[loWatchCountry.Name]) then
			if liInfluenceLeft == 0 then
				local loCancelCountry = nil
				
				if loWorstDistanceCountry then
					loCancelCountry = loWorstDistanceCountry
				elseif loWorstCountry then
					loCancelCountry = loWorstCountry
				end
				
				if loCancelCountry then
					if Support.ExecuteInfluence(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loCancelCountry.Tag, true) then
						liInfluenceLeft = liInfluenceLeft + 1
						lbProcess = false
					end
				end
			end

			-- Now Execute the Influence
			if liInfluenceLeft > 0 and loWatchCountry then
				if Support.ExecuteInfluence(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loWatchCountry.Tag) then
					liInfluenceLeft = liInfluenceLeft - 1
					lbProcess = false
				end
			end
		end
	end
	
	-- No Watch Influence this time so continue forward
	if lbProcess then
		-- If we are not actively Influencing anyone this code can be skiped
		if loLeaderShip.ActiveInfluence > 0 then
			-- We are using more than we take in so cancel lowest one
			if liInfluenceLeft <= 0 then
				local loCancelCountry = nil
				
				-- We are not watching anyone so Cancel worst one
				if not(loWatchCountry) and loWorstCountry then
					loCancelCountry = loWorstCountry
				elseif loWatchCountry then
					if loWorstCountry then
						if loWatchCountry.Tag ~= loWorstCountry.Tag then
							loCancelCountry = loWorstCountry
						elseif loSecondLowestCountry then
							loCancelCountry = loSecondLowestCountry
						end
					end
				end
			
				-- Short on Influence so Cancel something
				if loCancelCountry then
					if (liInfluenceLeft < 0)
					or 
					(loCancelCountry.Tag ~= loBestCountry.Tag and loLeaderShip.Diplomats >= liInfluenceCost) then
						if Support.ExecuteInfluence(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loCancelCountry.Tag, true) then
							liInfluenceLeft = liInfluenceLeft + 1
						end
					end
				end
			end
		end
		
		-- We are not influenceing our best option so
		if liInfluenceLeft > 0 
		and loLeaderShip.CanInfluence 
		and loBestCountry 
		and loLeaderShip.Diplomats >= liInfluenceCost then
			if not(loCurrentlyInfluencing[loBestCountry.Name]) then
				Support.ExecuteInfluence(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loBestCountry.Tag)
			end
		end
	end
end

function ForeignMinister_HandlePeace()
	-- Invite to Faction
	-- NAP (Non Aggression Pact)
	-- Guarantee 
	-- Allow Debt
	-- Alliance (Forming)
	-- Alliance (Breaking)
	-- Embargo (Making and Cancelling)	
	-- Offer Military Access
	-- Join Faction (or exit)

	-- Main Country processing loop
	for loTargetCountry in CCurrentGameState.GetCountries() do
		local loTargetCountryTag = loTargetCountry:GetCountryTag()

		if not(ForeignMinisterData.ministerCountry:HasDiplomatEnroute(loTargetCountryTag))
		and not(loTargetCountryTag == ForeignMinisterData.ministerTag) 
		and loTargetCountry:Exists()
		and loTargetCountryTag:IsReal()
		and loTargetCountryTag:IsValid() then
		
			local loRelation = ForeignMinisterData.ministerAI:GetRelation(ForeignMinisterData.ministerTag, loTargetCountryTag)
			local loReverseRelation = ForeignMinisterData.ministerAI:GetRelation(loTargetCountryTag, ForeignMinisterData.ministerTag)
			
			-- ONLY MAJOR POWERS CAN DO
			if ForeignMinisterData.IsMajor	then
				-- Calls in here Require Major be in a faction and target is not in a faction
				if ForeignMinisterData.HasFaction and not(loTargetCountry:HasFaction()) then
					local lsTargetCountryTag = tostring(loTargetCountryTag)
					
					-- Invite into faction
					local loAction = CFactionAction(ForeignMinisterData.ministerTag, loTargetCountryTag)
					loAction:SetValue(false)

					if loAction:IsSelectable() then
						local liScore = DiploScore_InviteToFaction(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag)
						
						if liScore > 50 then
							ForeignMinisterData.minister:Propose(loAction, liScore)
						end
					end			
				end

				
			end
			-- END OF MAJOR POWER ONLY
			
			--randomhoi all alliance
				if not(ForeignMinisterData.HasFaction) then
					if not(loRelation:HasAlliance()) and tonumber(tostring(loRelation:GetValue():GetTruncated())) > 0 then
						local liScore = DiploScore_Alliance(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, loTargetCountryTag)
						if liScore > 50 then
							local loAction = CAllianceAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
							if loAction:IsSelectable() then
								ForeignMinisterData.ministerAI:PostAction(loAction)
							end
						end
					end	
					
				else
					if ForeignMinisterData.ministerCountry:IsPuppet()  then --ally with your master, ALWAYS
						if not(loRelation:HasAlliance()) then
							if ForeignMinisterData.ministerCountry:IsPuppet() then
								local overlord = ForeignMinisterData.ministerCountry:GetOverlord():GetCountry()
								if loTargetCountryTag == overlord:GetCountryTag() then
									local loAction = CAllianceAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
									if loAction:IsSelectable() then
										ai:PostAction(loAction)
									end
								end
							end
						end
					end		
					
				end

		--randomhoi  Wars
			local loStrategy =  ForeignMinisterData.ministerCountry:GetStrategy()
			local warDesirability = -1
			
			local blAtWar=false
			
			for loTargetCountryWars in ForeignMinisterData.ministerCountry:GetCurrentAtWarWith() do
				if not loTargetCountryWars:GetCountry():IsGovernmentInExile() then
					blAtWar = true
					break;
				end
			end
			
			if not blAtWar and not loStrategy:IsPreparingWar() and not loTargetCountry:IsGovernmentInExile() then 
				for loRelation in ForeignMinisterData.ministerCountry:GetDiplomacy() do
					local loTargetTag = loRelation:GetTarget()
				 	
				 	--Utils.LUA_DEBUGOUT(tostring(loTargetTag))
				 	if math.random(4) == 1 then
				 		warDesirability = CalculateWarDesirability( ForeignMinisterData.ministerAI, 	ForeignMinisterData.ministerCountry, loTargetCountry  )
					end
						
				 	if warDesirability == 100 then
						loStrategy:PrepareWar( loTargetCountryTag, 100 )
						
						break
				 	end
			
			 	end
			 		 	
			else
			
				if ForeignMinisterData.Strategy:IsPreparingWarWith(loTargetCountryTag) and not 	ForeignMinisterData.ministerCountry:IsPuppet()  then -- if we are at war, cancel any declare war pending
							loStrategy:CancelPrepareWar( loTargetCountryTag )
				end		
						
			end
			
			if 	ForeignMinisterData.ministerCountry:IsPuppet() and not ForeignMinisterData.ministerCountry:IsAtWar() then --if you are a puppet, declare war to your master enemies. dont enter here if you are at war or you arent a puppet
				local overlord = ForeignMinisterData.ministerCountry:GetOverlord():GetCountry()
				if overlord:IsAtWar() then
					for loTargetCountryMW in overlord:GetCurrentAtWarWith() do -- we will save the day!
						 if not ForeignMinisterData.Strategy:IsPreparingWarWith(loTargetCountryMW) then
						 	loStrategy:PrepareWar( loTargetCountryMW, 100 )
						else
							break;	
						 end

					end
				end

			end

			-- Offer Military Access
			if ForeignMinisterData.ministerCountry:IsNonExileNeighbour(loTargetCountryTag) then
				if not(loRelation:HasMilitaryAccess()) and loTargetCountry:IsAtWar() then
					-- Make sure they are the same Ideology Group
					if loMinisterGroup == loTargetGroup then
						local loNeighborStrat = loTargetCountry:GetStrategy()
						
						-- Make sure they are not preparing for war with us
						if not(loNeighborStrat:IsPreparingWarWith(ForeignMinisterData.ministerTag)) then
							local loAction = COfferMilitaryAccessAction(ForeignMinisterData.ministerTag, loTargetCountryTag)

							if loAction:IsSelectable() then
								local liScore = DiploScore_OfferMilitaryAccess(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag)
								
								if liScore > 50 then
									ForeignMinisterData.minister:Propose(loAction, liScore)
								end
							end
						end
					end
				end
			end
			
			-- Cancels Military Access if relations to low
			if loReverseRelation:HasMilitaryAccess() then
				local loAction = CMilitaryAccessAction(loTargetCountryTag, ForeignMinisterData.ministerTag)
				loAction:SetValue(false)
				
				if loAction:IsSelectable() then
					local liScore = DiploScore_DemandMilitaryAccess(ForeignMinisterData.ministerAI, loTargetCountryTag, ForeignMinisterData.ministerTag, loTargetCountryTag)
					
					if liScore < 40 then
						ForeignMinisterData.minister:Propose(loAction, 100)
					end
				end
			end
			
			-- NAP-ing
			--   note: if the two have an alliance or part of the same faction (or aligning to the same side) dont bother with a NAP
			if not(loRelation:HasNap()) 
			and not(loRelation:HasAlliance())
			and not(ForeignMinisterData.Faction == loTargetCountry:GetFaction())then
				local loAction = CNapAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
				
				if loAction:IsSelectable() then
					local liScore = DiploScore_NonAgression(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, loTargetCountryTag)
					
					if liScore > 50 then
						ForeignMinisterData.minister:Propose(loAction, liScore)
					end
				end
			end

			-- Guarantee
			if not(loRelation:IsGuaranting()) then
				local loAction = CGuaranteeAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
				
				if loAction:IsSelectable() then
					local liScore = DiploScore_Guarantee(ForeignMinisterData, loTargetCountryTag)
					
					if liScore > 50 then
						ForeignMinisterData.minister:Propose(loAction, liScore)
					end
				end
			end

			-- Lend-Lease
			if (not ForeignMinisterData.ministerCountry:HasActiveLendLeaseFrom(loTargetCountryTag))
			and (not ForeignMinisterData.ministerCountry:IsGivingLendLeaseToTarget(loTargetCountryTag)) then
				local loAction = CRequestLendLeaseAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
				if loAction:IsSelectable() then
					local liScore = DiploScore_RequestLendLease(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, loTargetCountryTag, loAction)
					if liScore > 50 then
						ForeignMinisterData.minister:Propose(loAction, liScore)
					end
				end
			end

			--########################
			-- We are asked them to allow us the debt
			-- #######################
			-- Allow our Debt
			if not(loRelation:AllowDebts()) then
				-- ########################
				-- They asked us to allow them debt
				-- #######################
				-- Cancel Their Allow Debt
				if loReverseRelation:AllowDebts() then
					local loAction = CDebtAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
					loAction:SetValue(false)
					
					if loAction:IsSelectable() then
						local liScore = DiploScore_Debt( ForeignMinisterData.ministerAI, loTargetCountryTag, ForeignMinisterData.ministerTag, loTargetCountryTag )

						-- Cancel allow debt cause we have money to spend
						if liScore < 40 then
							ForeignMinisterData.minister:Propose(loAction, 100)
						end
					end
				else
					-- Our Debt
					local loAction = CDebtAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
					
					if loAction:IsSelectable() then
						local liScore = DiploScore_Debt( ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag )

						if liScore > 50 then
							ForeignMinisterData.minister:Propose(loAction, liScore)
						end
					end				
				end				
			-- Cancel Our Allow Debt
			else
				local loAction = CDebtAction(ForeignMinisterData.ministerTag, loTargetCountryTag)	
				loAction:SetValue(false)
				
				if loAction:IsSelectable() then
					local liScore = DiploScore_Debt( ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag )

					if liScore < 50 then
						ForeignMinisterData.minister:Propose(loAction, 100)
					end
				end
			
			end
			
			-- Embargo
			local loAction = CEmbargoAction(ForeignMinisterData.ministerTag, loTargetCountryTag)
			
			if loRelation:HasEmbargo() then
				-- do we want to stop embargoing?
				loAction:SetValue(false)
				
				if loAction:IsSelectable() then
					local liScore = DiploScore_Embargo(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag, false)

					if liScore < 30 then
						ForeignMinisterData.minister:Propose(loAction, 100)
					end
				end
				
			else
				if loAction:IsSelectable() then
					local liScore = DiploScore_Embargo(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag, true)

					if liScore > 60 then
						ForeignMinisterData.minister:Propose(loAction, liScore )
					end
				end
			end
		end
	end
	-- END OF MAIN LOOP
	
	-- Break Alliance
	if ForeignMinisterData.HasFaction then
		-- You are in a faction so break all your alliances and stay true to just your faction
		for loTargetCountryTag in ForeignMinisterData.ministerCountry:GetAllies() do
			local loAction = CAllianceAction(ForeignMinisterData.ministerTag, loTargetCountryTag)
			loAction:SetValue(false) -- cancel
			
			if loAction:IsSelectable() then
				ForeignMinisterData.minister:Propose(loAction, 100)
			end
		end		
	else
		for loTargetCountryTag in ForeignMinisterData.ministerCountry:GetAllies() do
			local liScore = DiploScore_BreakAlliance(ForeignMinisterData.ministerAI, ForeignMinisterData.ministerTag, loTargetCountryTag, ForeignMinisterData.ministerTag)

			if liScore >= 100 then
				local loAction = CAllianceAction(ForeignMinisterData.ministerTag, loTargetCountryTag)
				loAction:SetValue(false) -- cancel
				
				if loAction:IsSelectable() then
					ForeignMinisterData.minister:Propose(loAction, liScore )
				end
			end
		end		
	end
end
