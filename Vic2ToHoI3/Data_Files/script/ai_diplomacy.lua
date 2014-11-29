require('utils')

function CalculateAlignmentFactor(voAI, country1, country2)
	local dist = voAI:GetCountryAlignmentDistance( country1, country2 ):Get()
	return math.min(dist / 400.0, 1.0)
end

-- Calculates weather the AI should request (and accept) a Debt request
function DiploScore_Debt(voAI, voFromTag, voToTag, voObserverTag)
	local loDiploScoreObj = {
		Score = 0,
		ministerAI = voAI,
		ToTag = voToTag,
		FromTag = voFromTag,
		ToCountry = voToTag:GetCountry(),
		FromCountry = voFromTag:GetCountry()
	}
	
	if loDiploScoreObj.FromCountry:IsAtWar() then
		if loDiploScoreObj.FromCountry:HasFaction() then
			if loDiploScoreObj.FromCountry:GetFaction() == loDiploScoreObj.ToCountry:GetFaction() then
				local loReqMoney = loDiploScoreObj.FromCountry:GetPool():Get( CGoodsPool._MONEY_ ):Get()
				local loActMoney = loDiploScoreObj.ToCountry:GetPool():Get( CGoodsPool._MONEY_ ):Get()
				local liReqMoneyDaily = loDiploScoreObj.FromCountry:GetDailyBalance(CGoodsPool._MONEY_):Get()
				local liActMoneyDaily = loDiploScoreObj.ToCountry:GetDailyBalance(CGoodsPool._MONEY_):Get()
				
				-- Make sure they have a money reserve to even allow the debt
				if loReqMoney > loActMoney and liActMoneyDaily < 0.2 and liReqMoneyDaily > 0.2 then
					local liExpenseFactor, liHomeFactor = Support.CalculateExpenseResourceFactor(loDiploScoreObj.ToCountry)
					
					-- We are short on resources and have no money so ask to be allowed debt
					if liExpenseFactor > liHomeFactor then
						loDiploScoreObj.Score = 100
					end				
				end
			end
		end
	end
	
	return Utils.CallGetScoreAI(loDiploScoreObj.FromTag, 'DiploScore_Debt', loDiploScoreObj)
end

-- Calculates if the AI should embargo a country
function DiploScore_Embargo(voAI, voActorTag, voRecipientTag, voObserverTag, bEnabling)
	local loDiploScoreObj = {
		Score = 0,
		ministerAI = voAI,
		ministerTag = voActorTag,
		ministerCountry = voActorTag:GetCountry(),
		Faction = nil,
		IsAtWar = nil,
		EmbargoTag = voRecipientTag,
		EmbargoCountry = voRecipientTag:GetCountry(),
		EmbargoHasFaction = nil
	}	
	
	if loDiploScoreObj.ministerCountry:IsGovernmentInExile() or loDiploScoreObj.EmbargoCountry:IsGovernmentInExile() then
		return 0
	end
	
	loDiploScoreObj.EmbargoHasFaction = loDiploScoreObj.EmbargoCountry:HasFaction()
	loDiploScoreObj.Faction = loDiploScoreObj.EmbargoCountry:GetFaction()
	loDiploScoreObj.IsAtWar = loDiploScoreObj.ministerCountry:IsAtWar()
	
	--if loDiploScoreObj.IsAtWar then
	--	for loEnemyTag in loDiploScoreObj.ministerCountry:GetCurrentAtWarWith() do
	--		local loEnemyCountry = loEnemyTag:GetCountry()
	--		
	--		if loDiploScoreObj.EmbargoHasFaction then
	--			-- They are in the same faction as our enemy so embargo them
	--			if loEnemyCountry:GetFaction() == loDiploScoreObj.Faction then
	--				loDiploScoreObj.Score = 100 
	--				break
	--			end
	--		end
	--		
	--		-- They are allied with our enemy so embargo them
	--		if loEnemyCountry:GetRelation(loDiploScoreObj.EmbargoTag):HasAlliance() then
	--			loDiploScoreObj.Score = 100 
	--			break
	--		end
	--		
	--		-- They are a friend of one of enemies so embargo them
	--		if loDiploScoreObj.EmbargoCountry:IsFriend(loEnemyTag, true) then
	--			loDiploScoreObj.Score = 80 
	--			break
	--		end
	--	end
	--end

	-- dont use up the last of our points for this
	--if bEnabling and (loDiploScoreObj.ministerCountry:GetDiplomaticInfluence():Get() < (defines.diplomacy.EMBARGO_INFLUENCE_COST + 2)) then
	--	loDiploScoreObj.Score = loDiploScoreObj.Score / 2 - 1
	--end

	return Utils.CallGetScoreAI(loDiploScoreObj.ministerTag, 'DiploScore_Embargo', loDiploScoreObj)
end

-- Calculates if a Country is ready to join a faction
function DiploScore_InviteToFaction(voAI, voActorTag, voRecipientTag, voObserverTag)
	local loAllies = CCurrentGameState.GetFaction("allies")
	
	local loDiploScoreObj = {
		Score = 0,
		ministerAI = voAI,
		ministerTag = voActorTag,
		ministerCountry = voActorTag:GetCountry(),
		Faction = nil,
		IdeologyGroup = nil,
		IsAtWar = nil,
		TargetTag = voRecipientTag,
		TargetCountry = voRecipientTag:GetCountry(),
		TargetNeutrality = nil,
		TargetIdeologyGroup = nil,
		TargetIsAtWar = nil
	}	

	loDiploScoreObj.Faction = loDiploScoreObj.ministerCountry:GetFaction()
	loDiploScoreObj.IdeologyGroup = loDiploScoreObj.ministerCountry:GetRulingIdeology():GetGroup()
	loDiploScoreObj.IsAtWar = loDiploScoreObj.ministerCountry:IsAtWar()

	loDiploScoreObj.TargetIdeologyGroup = loDiploScoreObj.TargetCountry:GetRulingIdeology():GetGroup()
	loDiploScoreObj.TargetNeutrality = loDiploScoreObj.TargetCountry:GetEffectiveNeutrality():Get()
	loDiploScoreObj.TargetIsAtWar = loDiploScoreObj.TargetCountry:IsAtWar()
		
	-- The enemy of my enemy is my friend
	if loDiploScoreObj.TargetIsAtWar then
		for loDiploStatus in loDiploScoreObj.TargetCountry:GetDiplomacy() do
			local loTarget = loDiploStatus:GetTarget()
			
			if loTarget:IsValid() and loDiploStatus:HasWar() then
				-- We both have a war with the same country
				if loDiploScoreObj.ministerCountry:GetRelation(loTarget):HasWar() then
					loDiploScoreObj.Score = 50
					break
				end
			end					
		end
	end

	-- Calculate score depending on neutrality.
    -- The function  below is new. 
	------------------------------
    loDiploScoreObj.Score = loDiploScoreObj.Score + (100 - loDiploScoreObj.TargetNeutrality) * 
                                                    (100 - loDiploScoreObj.TargetNeutrality) * 300 / 10000
	-- only join allies if not maximum neutrality country. those join when attacked
	if (loDiploScoreObj.TargetCountry:GetTotalIC() < 50) and (loDiploScoreObj.Faction == loAllies) then
		if loDiploScoreObj.TargetCountry:GetNeutrality():Get() > 89 then
			loDiploScoreObj.Score = 0
		end
	end
	
	-- Same ideology so a small bonus
	if loDiploScoreObj.TargetIdeologyGroup == loDiploScoreObj.IdeologyGroup then
		-- check if safe (quick check if it can even happy to save call time
		if loDiploScoreObj.Score > 40 then
			if loDiploScoreObj.TargetCountry:GetTotalIC() > 50 or loDiploScoreObj.TargetCountry:IsLandSafeToJoin( loDiploScoreObj.Faction )  then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 25
			else
				loDiploScoreObj.Score = loDiploScoreObj.Score - 100
			end
		end
	else
		loDiploScoreObj.Score = loDiploScoreObj.Score - 80 -- we'll only join if attacked pretty much, bar a small chance
	end		

	loDiploScoreObj.Score = Utils.CallGetScoreAI(voRecipientTag, 'DiploScore_InviteToFaction', loDiploScoreObj)

	return loDiploScoreObj.Score
end

-- Calculates if two countries wish to form an Alliance
function DiploScore_Alliance(voAI, voActorTag, voRecipientTag, voObserverTag)
	local loDiploScoreObj = {
		Score = 0,
		ministerAI = voAI,
		ministerTag = voActorTag,
		ministerCountry = voActorTag:GetCountry(),
		HasFaction = nil,
		Faction = nil,
		IdeologyGroup = nil,
		IsAtWar = nil,
		TargetTag = voRecipientTag,
		TargetCountry = voRecipientTag:GetCountry(),
		TargetHasFaction = nil,
		TargetStrategy = nil,
		TargetNeutrality = nil,
		TargetIdeologyGroup = nil,
		TargetIsAtWar = nil,
		RelationObj = nil,
		Relation = nil
	}
	
	loDiploScoreObj.HasFaction = loDiploScoreObj.ministerCountry:HasFaction()
	loDiploScoreObj.TargetHasFaction = loDiploScoreObj.TargetCountry:HasFaction()
	
	-- Only consider alliances if neighter side is part of a faction
	if not(loDiploScoreObj.HasFaction) and not(loDiploScoreObj.TargetHasFaction) then
		-- They are our Vassal so force them in an alliance
		if loDiploScoreObj.TargetCountry:GetOverlord() == loDiploScoreObj.ministerTag then
			loDiploScoreObj.Score = 100
		else
			loDiploScoreObj.Faction = loDiploScoreObj.ministerCountry:GetFaction()
			loDiploScoreObj.IdeologyGroup = loDiploScoreObj.ministerCountry:GetRulingIdeology():GetGroup()
			loDiploScoreObj.IsAtWar = loDiploScoreObj.ministerCountry:IsAtWar()

			loDiploScoreObj.TargetIdeologyGroup = loDiploScoreObj.TargetCountry:GetRulingIdeology():GetGroup()
			loDiploScoreObj.TargetNeutrality = loDiploScoreObj.TargetCountry:GetEffectiveNeutrality():Get()
			loDiploScoreObj.TargetIsAtWar = loDiploScoreObj.TargetCountry:IsAtWar()

			loDiploScoreObj.TargetStrategy = loDiploScoreObj.TargetCountry:GetStrategy()
			loDiploScoreObj.RelationObj = loDiploScoreObj.TargetCountry:GetRelation(loDiploScoreObj.ministerTag)
			loDiploScoreObj.Relation = loDiploScoreObj.RelationObj:GetValue():GetTruncated()
		
		
			local lbFightingFriends = false -- If they are fighting our friends
			
			-- Check our Wars
			if loDiploScoreObj.IsAtWar then
				local lbMutualEnemies = false
				
				for loEnemyTag in loDiploScoreObj.ministerCountry:GetCurrentAtWarWith() do
					if loDiploScoreObj.TargetCountry:IsEnemy(loEnemyTag) then 
						lbMutualEnemies = true
					elseif loDiploScoreObj.TargetCountry:IsFriend(loEnemyTag, false) then
						lbFightingFriends = true
						break
					end
				end
				
				if lbMutualEnemies then
					loDiploScoreObj.Score = loDiploScoreObj.Score + 20
				else
					loDiploScoreObj.Score = loDiploScoreObj.Score / 2 -- better wait until they sorted their problems
				end
			end
		
		
			-- Check to see if relations are worth a bonus
			if loDiploScoreObj.Relation > 0 then
				loDiploScoreObj.Score = loDiploScoreObj.Score + (loDiploScoreObj.Relation / 10.0)
			else
				loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.Relation
			end

			-- If part of the same ideology group small bonus
			if loDiploScoreObj.TargetIdeologyGroup == loDiploScoreObj.IdeologyGroup then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 10
			else
				loDiploScoreObj.Score = loDiploScoreObj.Score - 30
			end

			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.TargetCountry:GetDiplomaticDistance(loDiploScoreObj.ministerTag):GetTruncated() / 10
			loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.TargetStrategy:GetFriendliness(loDiploScoreObj.ministerTag) / 2
			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.TargetStrategy:GetAntagonism(loDiploScoreObj.ministerTag) / 2
			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.RelationObj:GetThreat():Get() / 2
			
			-- If not neighbors and capital is not on the same continent
			if not loDiploScoreObj.TargetCountry:IsNonExileNeighbour(loDiploScoreObj.ministerTag) then
				-- check if on same continent first
				local loTargetCont = loDiploScoreObj.TargetCountry:GetActingCapitalLocation():GetContinent()
				local loMinisterCont = loDiploScoreObj.ministerCountry:GetActingCapitalLocation():GetContinent()
				
				if not (loTargetCont == loMinisterCont) then
					loDiploScoreObj.Score = loDiploScoreObj.Score / 2
				end
			end

		    if lbFightingFriends then
				loDiploScoreObj.Score = loDiploScoreObj.Score / 2 -- better wait until they sorted their problems
			end
			
			return Utils.CallGetScoreAI(loDiploScoreObj.TargetTag, 'DiploScore_Alliance', loDiploScoreObj)
		end
	end
	
	return loDiploScoreObj.Score
end

function DiploScore_NonAgression(voAI, voActorTag, voRecipientTag, voObserverTag)
	if voObserverTag == voActorTag then -- we demand nap with voRecipientTag
		return DiploScore_NonAgression(voAI, voRecipientTag, voActorTag, voObserverTag)
	else -- voActorTag demands nap with us
		local score = 0
		local rel = voAI:GetRelation(voRecipientTag, voActorTag)
		local relation = 100 + rel:GetValue():GetTruncated()
		
		if relation > 150 then -- we like them
			score = score + (relation - 150)
		elseif voAI:GetNumberOfOwnedProvinces(voActorTag) / 2 >
		       voAI:GetNumberOfOwnedProvinces(voRecipientTag) then -- much bigger than us
			score = score + 5 + relation / 5
		end
		
		local recipientCountry = voRecipientTag:GetCountry()
		local strategy = recipientCountry:GetStrategy()
		score = score + strategy:GetFriendliness(voActorTag) / 4
		score = score - strategy:GetAntagonism(voActorTag) / 4
		--score = score + strategy:GetThreat(voActorTag) / 4
		
		if not recipientCountry:IsNonExileNeighbour( voActorTag ) then
			score = score / 2
		end

		score = score - recipientCountry:GetDiplomaticDistance(voActorTag):GetTruncated() 

		return Utils.CallScoredCountryAI(voRecipientTag, 'DiploScore_NonAgression', score, voAI, voActorTag, voRecipientTag, voObserverTag)
	end
end





function CalculateWarDesirability(voAI, country, target)
	local score = 0
	local countryTag = country:GetCountryTag()
	local targetCountry = target:GetCountry()
	local strategy = country:GetStrategy()

	-- can we even declare war?
	if not voAI:CanDeclareWar( countryTag, target ) then
	  return 0
	end

	--Utils.LUA_DEBUGOUT("we can declare war: " .. tostring(minister:GetCountryTag()) .. " -> " .. tostring(target) )


	local antagonism = strategy:GetAntagonism(target);
	local friendliness = strategy:GetFriendliness(target);

	-- dont declare war on people we like
	if friendliness > 0 and antagonism < 1 then
		return 0
	end

	-- no suicide :S
	if country:GetNumberOfControlledProvinces() < targetCountry:GetNumberOfControlledProvinces() / 4 then
		return 0
	end

	-- watch out if we have bad intel, should be infiltrating more
	local intel = CAIIntel(countryTag, target)
	if intel:GetFactor() < 0.1 then
		return 0
	end

	-- compare military power
	local theirStrength = intel:CalculateTheirPercievedMilitaryStrengh()
	local ourStrength = intel:CalculateOurMilitaryStrength()
	local strengthFactor = ourStrength / theirStrength

	if strengthFactor < 1.0 then
		score = score - 75 * (1.0 - strengthFactor)
	else
		score = score + 20 * (strengthFactor - 1.0)
	end

	-- personality
	if strategy:IsMilitarist() then
		score = score * 1.3
	end
	
	return Utils.CallScoredCountryAI(countryTag, 'CalculateWarDesirability', score, voAI, country, target)

end

function DiploScore_PeaceAction(voAI, voActorTag, voRecipientTag, voObserverTag, action)
	if voObserverTag == voActorTag then
		return 0
	else
		score = 0
		
		-- intel first
		--Utils.LUA_DEBUGOUT("----------")
		local intel = CAIIntel(voRecipientTag, voActorTag)
		if intel:GetFactor() > 0.1 then
			local recipientStrength = intel:CalculateTheirPercievedMilitaryStrengh()
			local actorStrength = intel:CalculateOurMilitaryStrength()
			local strengthFactor = actorStrength / recipientStrength - 0.5
			score = 100 * strengthFactor
		end
		--Utils.LUA_DEBUGOUT("score: " .. score )
		
		local sizeFactor = voActorTag:GetCountry():GetNumberOfControlledProvinces() / voRecipientTag:GetCountry():GetNumberOfControlledProvinces()
		--Utils.LUA_DEBUGOUT("sizeFactor: " .. sizeFactor )
		sizeFactor = (sizeFactor - 1) * 100
				
		score = score + math.min(sizeFactor, 100)
		
		score = score + voRecipientTag:GetCountry():GetSurrenderLevel():Get() * 100
		--Utils.LUA_DEBUGOUT("score: " .. score )
		score = score - voActorTag:GetCountry():GetSurrenderLevel():Get() * 100
		--Utils.LUA_DEBUGOUT("score: " .. score )
		
		local strategy = voRecipientTag:GetCountry():GetStrategy()
		score = score + strategy:GetFriendliness(voActorTag) / 2
		score = score - strategy:GetAntagonism(voActorTag) / 2
		--score = score + strategy:GetThreat(voActorTag) / 2
		--Utils.LUA_DEBUGOUT("score: " .. score )
		return score
	end
end

function DiploScore_SendExpeditionaryForce(voAI, voActorTag, voRecipientTag, voObserverTag, action)
	if voObserverTag == voActorTag then
		return 0 
	else
		local  score = 0
		-- do we want to accept?
		local recipientCountry = voRecipientTag:GetCountry()
		if recipientCountry:GetDailyBalance( CGoodsPool._SUPPLIES_ ):Get() > 1.0 then
			-- maybe we have enough stockpiles
			local supplyStockpile = recipientCountry:GetPool():Get( CGoodsPool._SUPPLIES_ ):Get()
			local weeksSupplyUse = recipientCountry:GetDailyExpense( CGoodsPool._SUPPLIES_ ):Get() * 7
			if supplyStockpile > weeksSupplyUse * 20.0 then
				score = score + 70
			elseif supplyStockpile > weeksSupplyUse * 10.0 then
				score = score + 40
			end
			
			if recipientCountry:IsAtWar() then
				score = score + 20
			else
				score = 0 -- no war, no need for troops
			end
		end
		
		return score
	end
end

function DiploScore_CallAlly(voAI, voActorTag, voRecipientTag, voObserverTag, action)
	if voObserverTag == voActorTag then
		return 100
	else
		local loDiploScoreObj = {
			Score = 0,
			ministerAI = voAI,
			ActorTag = voActorTag,
			ActorCountry = voActorTag:GetCountry(),
			ActorFaction = nil,
			ActorFactionTag = nil,
			RecipientTag = voRecipientTag,
			RecipientCountry = voRecipientTag:GetCountry(),
			ReceipientFaction = nil,
			ReceipientFactionTag = nil,
			ReceipientMasterTag = nil
		}
	
		loDiploScoreObj.ActorFaction = loDiploScoreObj.ActorCountry:GetFaction()
		loDiploScoreObj.ActorFactionTag = tostring(loDiploScoreObj.ActorFaction:GetTag())
		loDiploScoreObj.ReceipientFaction = loDiploScoreObj.RecipientCountry:GetFaction()
		loDiploScoreObj.ReceipientFactionTag = tostring(loDiploScoreObj.ReceipientFaction:GetTag())
		loDiploScoreObj.ReceipientMasterTag = loDiploScoreObj.RecipientCountry:GetOverlord()

		-- Are they in a faction
		if loDiploScoreObj.ActorFaction == loDiploScoreObj.ReceipientFaction
		or loDiploScoreObj.ReceipientMasterTag == loDiploScoreObj.ActorTag then
			loDiploScoreObj.Score = 100
		else
			-- Must be an alliance so return alliance score
			loDiploScoreObj.Score = DiploScore_Alliance(voAI, voActorTag, voRecipientTag, voObserverTag, nil)
		end
		
		loDiploScoreObj.Score = Utils.CallGetScoreAI(voRecipientTag, "DiploScore_CallAlly", loDiploScoreObj)
		return loDiploScoreObj.Score
	end
end


function DiploScore_OfferLendLease(voAI, voActorTag, voRecipientTag, voObserverTag)
	if voObserverTag == voRecipientTag then
		return 100
	else
		return 0
	end
end

function DiploScore_RequestLendLease(voAI, voActorTag, voRecipientTag, voObserverTag, action)
	if voObserverTag == voActorTag then
		return 100 -- hey we always want more ic but this shouldnt really be called
	else -- check if recipient wants to give actor LL. 
		local actorCountry = voActorTag:GetCountry()
		local recipientCountry = voRecipientTag:GetCountry()
		local liScore = 0
		
		if recipientCountry:IsEnemy( voActorTag ) then
			liScore = 0;
		else
			-- player?
			local lbActorIsPlayer = CCurrentGameState.IsPlayer( voActorTag )
			local lvMaxRecipientIC = recipientCountry:GetMaxIC()
			local lvMaxActorIC = actorCountry:GetMaxIC() 

			if lbActorIsPlayer or (lvMaxRecipientIC > 100 and lvMaxActorIC > 80) then
				-- we wont share if we have less
				if (lvMaxRecipientIC * 0.8) > lvMaxActorIC then
					local voRecipientFaction = recipientCountry:GetFaction()
					if not voRecipientFaction:IsValid() then -- closest then?
						
						local loClosestFaction = nil
						local vSmallestAlignment = 100000
						for loFaction in CCurrentGameState.GetFactions() do
								
							local liAlignment = voAI:GetCountryAlignmentDistance(recipientCountry, loFaction:GetFactionLeader():GetCountry()):Get()
							if liAlignment < vSmallestAlignment then
								vSmallestAlignment = liAlignment
								loClosestFaction = loFaction
							end
						end
							
						if vSmallestAlignment < 75 then
							voRecipientFaction = loClosestFaction
						end
					end

					-- same faction?
					if voRecipientFaction:IsValid() then 
						if (actorCountry:GetFaction() == voRecipientFaction) then
							liScore = 100
						elseif actorCountry:HasFaction() then -- are they fighting a common enemy
							for loEnemyTag in actorCountry:GetCurrentAtWarWith() do
								local loEnemyCountry = loEnemyTag:GetCountry()
								if loEnemyCountry:HasFaction() then
									if loEnemyCountry:IsEnemy( voRecipientFaction:GetFactionLeader() ) then
										liScore = 80
									end
									break
								end							
							end
						end
					end
				end
			end
		end

		return Utils.CallScoredCountryAI(voRecipientTag, "DiploScore_RequestLendLease", liScore, voAI, voActorTag )
	end
end

-- #######################
-- Military Access
-- #######################
function DiploScore_DemandMilitaryAccess(voAI, voActorTag, voRecipientTag, voObserverTag)
	-- Actor and Observer are same thing when demanding
	local liScore = Generate_MilitaryAccess_Score(voAI, voActorTag, voRecipientTag)
	
	-- Same as Offer but reverse the voActorTag and voRecipientTag
	--   Only pass the country that is asking for access
	liScore = Utils.CallScoredCountryAI(voRecipientTag, "DiploScore_GiveMilitaryAccess", liScore, voAI, voActorTag)			

	return liScore
end

function DiploScore_OfferMilitaryAccess(voAI, voActorTag, voRecipientTag, voObserverTag, action)
	-- Recipient and Observer are same thing when Offering
	local liScore = Generate_MilitaryAccess_Score(voAI, voRecipientTag, voActorTag)
	
	-- Offer scores have a natural penalty so the AI will not consider them
	liScore = liScore - 25
	
	-- Same as Offer but reverse the voActorTag and voRecipientTag
	--   Only pass the country that we are considering giving access to
	liScore = Utils.CallScoredCountryAI(voActorTag, "DiploScore_GiveMilitaryAccess", liScore, voAI, voRecipientTag)			

	return liScore	
end

function Generate_MilitaryAccess_Score(voAI, voActorTag, voRecipientTag)
	local liScore = 0
	local loRelation = voAI:GetRelation(voRecipientTag, voActorTag)
	
	-- If they are atwar with eachother this is impossible
	if not(loRelation:HasWar()) then
		local loRecipientCountry = voRecipientTag:GetCountry()
		local loActorCountry = voActorTag:GetCountry()
		
		-- Asking or Demanding as a puppet is not possible for military access
		if not(loRecipientCountry:IsPuppet()) and not(loActorCountry:IsPuppet()) then
			-- If they are in a faction then exit
			if not(loRecipientCountry:HasFaction()) then
				local lbMajorNeighborCheck = false
				local loRecipientGroup = loRecipientCountry:GetRulingIdeology():GetGroup()
				local loActorGroup = loActorCountry:GetRulingIdeology():GetGroup()
			
				-- Same ideology so a small bonus
				if loRecipientGroup == loActorGroup then
					liScore = liScore + 25
				else
					liScore = liScore - 10
				end
				
				-- Check to see who they are after, if it is another major do not get involved
				for loCountryTag in loRecipientCountry:GetNeighbours() do
					local loRelation2 = voAI:GetRelation(voActorTag, loCountryTag)
					
					if loRelation2:HasWar() then
						local loCountry2 = loCountryTag:GetCountry()
						if loCountry2:IsMajor() then
							lbMajorNeighborCheck = true
							liScore = liScore - 25
							break
						end
					end
				end
				
				-- They are after a minor so go ahead and give them a small bonus
				if not(lbMajorNeighborCheck) then
					liScore = liScore + 25
				end

				-- Calculate strength based on IC
				--   The smaller the minor the more likely they will say yes
				local liRecipientIC = loRecipientCountry:GetMaxIC()
				local liActorIC = loActorCountry:GetMaxIC()
				
				if liActorIC > (liRecipientIC * 7) then
					liScore = liScore + 25
				elseif liActorIC > (liRecipientIC * 5) then
					liScore = liScore + 10
				elseif liActorIC > (liRecipientIC * 3) then
					liScore = liScore + 5
				end

				-- If they are heavily neutral then don't let them through
				local liEffectiveNeutrality = loRecipientCountry:GetEffectiveNeutrality():Get()
				if liEffectiveNeutrality > 90 then
					liScore = liScore - 50
				elseif liEffectiveNeutrality > 80 then
					liScore = liScore - 25
				elseif liEffectiveNeutrality > 70 then
					liScore = liScore - 10
				end
				
				-- Now Calculate Threat and Relations into the score
				liScore = liScore - loRelation:GetThreat():Get() / 5
				liScore = liScore + loRelation:GetValue():GetTruncated() / 3
			end
		end
	end
	
	return liScore
end
-- #######################


-- ############################
--  Methods that are called directly from ai_foreign_minister.lua and never by the EXE
-- ############################

-- Calculates score of the requested country for influencing
function DiploScore_InfluenceNation(voForeignMinisterData, voTargetCountryTag)
	-- Setup Diplo Object to be consistent
	local loDiploScoreObj = {
		Score = 500,
		ministerAI = voForeignMinisterData.ministerAI,
		ministerTag = voForeignMinisterData.ministerTag,
		ministerCountry = voForeignMinisterData.ministerCountry,
		HasFaction = voForeignMinisterData.HasFaction,
		Faction = voForeignMinisterData.Faction,
		FactionName = voForeignMinisterData.FactionName,
		Year = voForeignMinisterData.Year,
		Month = voForeignMinisterData.Month,
		Day = voForeignMinisterData.Day,
		FactionLeader = nil,
		FactionLeaderCountry = nil,
		IsAtWar = voForeignMinisterData.IsAtWar,
		Strategy = voForeignMinisterData.Strategy,
		IdeologyGroup = nil,
		
		TargetTag = voTargetCountryTag,
		TargetName = tostring(voTargetCountryTag),
		TargetCountry = voTargetCountryTag:GetCountry(),
		TargetNeutrality = nil, -- Their current neutrality
		TargetIdeologyGroup = nil, -- Ideology of the targeted country
		TargetIsMajor = nil, -- Are they a major power
		
		IsNeighbour = nil, -- Are the two countries neighbors
		AlignDistance = nil, -- How close they are aligned to our faction
		RelationObj = nil
	}
	
	loDiploScoreObj.FactionLeader = loDiploScoreObj.Faction:GetFactionLeader()
	loDiploScoreObj.FactionLeaderCountry = loDiploScoreObj.FactionLeader:GetCountry()
	loDiploScoreObj.AlignDistance = loDiploScoreObj.ministerAI:GetCountryAlignmentDistance(loDiploScoreObj.TargetCountry, loDiploScoreObj.FactionLeaderCountry):Get()
	
	-- Performance Check, are they already in our corner if so exit out do not influence
	if loDiploScoreObj.AlignDistance <= 10 then
		loDiploScoreObj.Score = 0
	else
		-- Load these only if we need to (Performance)
		loDiploScoreObj.IdeologyGroup = loDiploScoreObj.ministerCountry:GetRulingIdeology():GetGroup()
		loDiploScoreObj.TargetIdeologyGroup = loDiploScoreObj.TargetCountry:GetRulingIdeology():GetGroup()
		loDiploScoreObj.TargetIsMajor = loDiploScoreObj.TargetCountry:IsMajor()
		loDiploScoreObj.TargetNeutrality = loDiploScoreObj.TargetCountry:GetEffectiveNeutrality():Get()
		loDiploScoreObj.IsNeighbour = loDiploScoreObj.ministerCountry:IsNeighbour(loDiploScoreObj.TargetTag)
		loDiploScoreObj.RelationObj = loDiploScoreObj.ministerAI:GetRelation(loDiploScoreObj.ministerTag, loDiploScoreObj.TargetTag)
	
		-- Calculate Importance based on IC
		---   Remember on Majors can Influence
		local liMaxIC = loDiploScoreObj.TargetCountry:GetMaxIC()
		local liOurMaxIC = loDiploScoreObj.ministerCountry:GetMaxIC()
		
		if liMaxIC > liOurMaxIC then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 70
		elseif liMaxIC > liOurMaxIC * 0.5 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 40
		elseif liMaxIC > liOurMaxIC * 0.3 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 30
		elseif liMaxIC > liOurMaxIC * 0.2 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 20
		elseif liMaxIC > liOurMaxIC * 0.1 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		elseif liMaxIC > liOurMaxIC * 0.05 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- The closer they are to entering the war the higher priority to influence them out
		if loDiploScoreObj.TargetNeutrality > 90 then
			loDiploScoreObj.Score = loDiploScoreObj.Score - 100
		elseif loDiploScoreObj.TargetNeutrality > 80 then
			loDiploScoreObj.Score = loDiploScoreObj.Score - 70
		elseif loDiploScoreObj.TargetNeutrality > 70 then
			loDiploScoreObj.Score = loDiploScoreObj.Score - 10
		elseif loDiploScoreObj.TargetNeutrality < defines.diplomacy.JOIN_FACTION_NEUTRALITY + 10 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 50
		end
		
		-- Political checks
		-- ################
		loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.RelationObj:GetThreat():Get() / 5
		loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.RelationObj:GetValue():GetTruncated() / 3
		
		if loDiploScoreObj.RelationObj:IsGuaranteed() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		if loDiploScoreObj.RelationObj:HasFriendlyAgreement() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		if loDiploScoreObj.RelationObj:AllowDebts() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		-- ################
		
		-- We are neighbors
		if loDiploScoreObj.IsNeighbour then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 50
		else
			local loContinent = loDiploScoreObj.ministerCountry:GetActingCapitalLocation():GetContinent()
			local loTargetContinent = loDiploScoreObj.TargetCountry:GetActingCapitalLocation():GetContinent()
			
			-- We are on the same continent so bonus
			if loContinent == loTargetContinent then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 40
			end
		end
		
		-- They are already leaning toward us 
		if Support.IsFriend(loDiploScoreObj.ministerAI, loDiploScoreObj.Faction, loDiploScoreObj.TargetCountry) then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 20
		else
			loDiploScoreObj.Score = loDiploScoreObj.Score - 20
		end
		
		-- They are a major power
		if loDiploScoreObj.TargetIsMajor then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		-- They have a neighbor that is in our faction already
		if loDiploScoreObj.TargetCountry:HasNeighborInFaction(loDiploScoreObj.Faction) then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 20
		end
		if loDiploScoreObj.IdeologyGroup ~= loDiploScoreObj.TargetIdeologyGroup then
			loDiploScoreObj.Score = loDiploScoreObj.Score - 15
		else
			loDiploScoreObj.Score = loDiploScoreObj.Score + 15
		end
		
		loDiploScoreObj.Score = Utils.CallGetScoreAI(loDiploScoreObj.ministerTag, 'DiploScore_InfluenceNation', loDiploScoreObj)
	end
	
	return loDiploScoreObj.Score
end

-- Calculates score if we should guarantee a country
function DiploScore_Guarantee(voForeignMinisterData, voTargetCountryTag)
	-- Setup Diplo Object to be consistent
	local loDiploScoreObj = {
		Score = 0,
		ministerAI = voForeignMinisterData.ministerAI,
		ministerTag = voForeignMinisterData.ministerTag,
		ministerCountry = voForeignMinisterData.ministerCountry,
		HasFaction = voForeignMinisterData.HasFaction,
		Faction = voForeignMinisterData.Faction,
		IsAtWar = voForeignMinisterData.IsAtWar,
		IsMajor = voForeignMinisterData.IsMajor,
		Strategy = voForeignMinisterData.Strategy,
		IdeologyGroup = nil,
		HomeContinent = nil, -- Continent Location of their capital
		
		TargetTag = voTargetCountryTag,
		TargetCountry = voTargetCountryTag:GetCountry(),
		TargetHasFaction = nil,
		TargetFaction = nil,
		TargetIsAtWar = nil,
		TargetIsMajor = nil, -- Are they a major power
		TargetNeutrality = nil, -- Their current neutrality
		TargetIdeologyGroup = nil, -- Ideology of the targeted country
		TargetHomeContinent = nil, -- Continent Location of their capital

		IsNeighbour = nil, -- Are the two countries neighbors
		RelationObj = nil,
		Relation = nil
	}
	
	loDiploScoreObj.TargetHasFaction = loDiploScoreObj.TargetCountry:HasFaction()

	-- Performance Check
	-- If both parties are in factiosn no point in a guarantee
	--   if the target is in exile dont do it either
	if not(loDiploScoreObj.TargetHasFaction and loDiploScoreObj.HasFaction)
	and not(loDiploScoreObj.TargetCountry:IsGovernmentInExile()) then
		loDiploScoreObj.IdeologyGroup = loDiploScoreObj.ministerCountry:GetRulingIdeology():GetGroup()
		loDiploScoreObj.HomeContinent = loDiploScoreObj.ministerCountry:GetActingCapitalLocation():GetContinent()

		loDiploScoreObj.TargetFaction = loDiploScoreObj.TargetCountry:GetFaction()
		loDiploScoreObj.TargetIsAtWar = loDiploScoreObj.TargetCountry:IsAtWar()
		loDiploScoreObj.TargetIdeologyGroup = loDiploScoreObj.TargetCountry:GetRulingIdeology():GetGroup()
		loDiploScoreObj.TargetIsMajor = loDiploScoreObj.TargetCountry:IsMajor()
		loDiploScoreObj.TargetNeutrality = loDiploScoreObj.TargetCountry:GetEffectiveNeutrality():Get()
		loDiploScoreObj.TargetHomeContinent = loDiploScoreObj.TargetCountry:GetActingCapitalLocation():GetContinent()
		loDiploScoreObj.IsNeighbour = loDiploScoreObj.ministerCountry:IsNeighbour(loDiploScoreObj.TargetTag)
		loDiploScoreObj.RelationObj = loDiploScoreObj.ministerAI:GetRelation(loDiploScoreObj.ministerTag, loDiploScoreObj.TargetTag)
		loDiploScoreObj.Relation = loDiploScoreObj.RelationObj:GetValue():GetTruncated()

		-- Same ideology so small bonus
		if loDiploScoreObj.TargetIdeologyGroup == loDiploScoreObj.IdeologyGroup then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- If they are not a major power small bonus
		if not(loDiploScoreObj.TargetIsMajor) then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- We are neighbors so small bonus
		if loDiploScoreObj.IsNeighbour then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- We are on the same continent so bonus
		if loDiploScoreObj.HomeContinent == loDiploScoreObj.TargetHomeContinent then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end

		-- Diplomacy checks to see if we are friendly to eachother
		if loDiploScoreObj.RelationObj:IsGuaranteed() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		if loDiploScoreObj.RelationObj:HasFriendlyAgreement() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		if loDiploScoreObj.RelationObj:AllowDebts() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- Now add our relations
		if loDiploScoreObj.Relation > 10 then
			loDiploScoreObj.Score = loDiploScoreObj.Score + (loDiploScoreObj.Relation / 10)
		else
			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.Relation
		end
		
		loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.Strategy:GetFriendliness(loDiploScoreObj.TargetTag) / 2
		loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.Strategy:GetProtectionism(loDiploScoreObj.TargetTag)
		loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.Strategy:GetAntagonism(loDiploScoreObj.TargetTag) / 2
		loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.TargetCountry:GetDiplomaticDistance(loDiploScoreObj.ministerTag):GetTruncated() 

		loDiploScoreObj.Score = Utils.CallGetScoreAI(loDiploScoreObj.ministerTag, 'DiploScore_Guarantee', loDiploScoreObj)
		
		-- Clamp down, if the score is not over 70 do not guarantee as it should be rare
		if loDiploScoreObj.Score < 70 then
			loDiploScoreObj.Score = 0
		end
	end
	
	return loDiploScoreObj.Score
end

function DiploScore_BreakAlliance(voAI, voActorTag, voRecipientTag, voObserverTag)
	local liScore = 0

	if voActorTag == voObserverTag then
		local actorCountry = voActorTag:GetCountry()
		local recipientCountry = voRecipientTag:GetCountry()
		local loStrategy = actorCountry:GetStrategy()

		local loRelations = actorCountry:GetRelation(voRecipientTag)
		local liRelationValue = loRelations:GetValue():GetTruncated()

		local liThreat = loRelations:GetThreat():Get() * CalculateAlignmentFactor(voAI, actorCountry, recipientCountry)
		local liAntagonism = loStrategy:GetAntagonism(voRecipientTag) / 4
		local liFriendliness = loStrategy:GetFriendliness(voRecipientTag) / 4

		liScore = ((liAntagonism + liThreat) / 2.0) - liFriendliness
		liScore = liScore - liRelationValue / 2.0
	end
	
	return liScore
end

function DiploScore_JoinFactionGoal(voAI, voActorTag, voRecipientTag, voObserverTag, voWarGoal)
	local score = 0

	score = Utils.CallScoredCountryAI(voObserverTag, 'DiploScore_JoinFactionGoal', score, voAI, voActorTag, voRecipientTag, voObserverTag, voWarGoal)
	return score
end
