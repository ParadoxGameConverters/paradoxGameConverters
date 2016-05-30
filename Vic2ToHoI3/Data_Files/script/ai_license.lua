
local P = {}
Support_License = P

-- ###########################
-- Called by the EXE
-- ###########################
function DiploScore_LicenceTechnology(voAI, voActorTag, voRecipientTag, voObserverTag, voCommand)
	local loDiploScoreObj = {
		Score = 0,
		DefaultScore = 50,
		ministerAI = voAI,
		Command = voCommand,
		SubUnit = voCommand:GetSubunit(),
		Cost = 0,
		TotalCost = 0,
		Parallel = voCommand:GetParalell(),
		Serial = voCommand:GetSerial(),
		UnitCount = 0,
		IsNeighbor = nil,
		Relation = nil,
		Money = nil,
		Seller = {
			Name = tostring(voRecipientTag),
			Tag = voRecipientTag,
			Country = voRecipientTag:GetCountry(),
			Faction = nil,
			Continent = nil,
			Ideology = nil,
			HasFaction = nil,
			Pool = 0},
		Buyer = {
			TagName = tostring(voActorTag),
			Tag = voActorTag,
			Country = voActorTag:GetCountry(),
			Faction = nil,
			Continent = nil,
			Ideology = nil,
			Strategy = nil}}
	
	-- They did not select a unit so return 0
	if not(loDiploScoreObj.SubUnit) then
		return 0
	end
	
	loDiploScoreObj.Buyer.Faction = loDiploScoreObj.Buyer.Country:GetFaction()
	loDiploScoreObj.Seller.Faction = loDiploScoreObj.Seller.Country:GetFaction()
	loDiploScoreObj.Buyer.Ideology = tostring(loDiploScoreObj.Buyer.Country:GetRulingIdeology():GetGroup():GetKey())
	loDiploScoreObj.Seller.Ideology = tostring(loDiploScoreObj.Seller.Country:GetRulingIdeology():GetGroup():GetKey())
	loDiploScoreObj.Buyer.HasFaction = loDiploScoreObj.Buyer.Country:HasFaction()
	
	loDiploScoreObj.AlignDist = math.floor(loDiploScoreObj.ministerAI:GetCountryAlignmentDistance(loDiploScoreObj.Buyer.Country, loDiploScoreObj.Seller.Country):Get())

	if (loDiploScoreObj.Buyer.HasFaction and loDiploScoreObj.Buyer.Faction == loDiploScoreObj.Seller.Faction)
	or (loDiploScoreObj.Buyer.Ideology == loDiploScoreObj.Seller.Ideology and loDiploScoreObj.AlignDist < 125)
	or (loDiploScoreObj.AlignDist < 80) then
		-- Get Cost Per Unit
		loDiploScoreObj.UnitCount = loDiploScoreObj.Parallel * loDiploScoreObj.Serial
		loDiploScoreObj.Buyer.Pool = loDiploScoreObj.Buyer.Country:GetPool():Get(CGoodsPool._MONEY_):Get()
		loDiploScoreObj.Cost = P.CalculateCost(loDiploScoreObj.Buyer, loDiploScoreObj.Seller, loDiploScoreObj.SubUnit)
		loDiploScoreObj.TotalCost = loDiploScoreObj.Cost * loDiploScoreObj.UnitCount
		loDiploScoreObj.Money = loDiploScoreObj.Command:GetMoney():Get() * loDiploScoreObj.UnitCount

		if loDiploScoreObj.TotalCost > loDiploScoreObj.Buyer.Pool
		or loDiploScoreObj.TotalCost > loDiploScoreObj.Money then
			return 0
		else
			loDiploScoreObj.Relation = loDiploScoreObj.ministerAI:GetRelation(loDiploScoreObj.Buyer.Tag, loDiploScoreObj.Seller.Tag)
			loDiploScoreObj.Seller.Strategy = loDiploScoreObj.Seller.Country:GetStrategy()
			loDiploScoreObj.IsNeighbor = loDiploScoreObj.Buyer.Country:IsNonExileNeighbour(loDiploScoreObj.Seller.Tag)	
			loDiploScoreObj.Continent = loDiploScoreObj.Seller.Country:GetActingCapitalLocation():GetContinent()
			loDiploScoreObj.Continent = loDiploScoreObj.Buyer.Country:GetActingCapitalLocation():GetContinent()

			loDiploScoreObj.Score = loDiploScoreObj.DefaultScore -- Set it to default
			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.Seller.Strategy:GetAntagonism(loDiploScoreObj.Buyer.Tag) / 15			
			loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.Seller.Strategy:GetFriendliness(loDiploScoreObj.Buyer.Tag) / 10
			loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.Relation:GetThreat():Get() / 5
			loDiploScoreObj.Score = loDiploScoreObj.Score + tonumber(tostring(loDiploScoreObj.Relation:GetValue():GetTruncated())) / 3
			
			if loDiploScoreObj.Buyer.HasFaction then
				if loDiploScoreObj.Buyer.Faction == loDiploScoreObj.Seller.Faction then
					loDiploScoreObj.Score = loDiploScoreObj.Score + 30
				end
			end
			
			if loDiploScoreObj.Relation:IsGuaranteed() then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 5
			end
			if loDiploScoreObj.Relation:HasFriendlyAgreement() then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 15
			end
			if loDiploScoreObj.Relation:IsFightingWarTogether() then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 25
			end
			
			if loDiploScoreObj.IsNeighbor then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 5
			end
			
			if loDiploScoreObj.Seller.Continent == loDiploScoreObj.Buyer.Continent then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 5
			end
			
			if loDiploScoreObj.Buyer.Ideology == loDiploScoreObj.Seller.Ideology then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 15
			end
			
			if Support.IsFriend(loDiploScoreObj.ministerAI, loDiploScoreObj.Buyer.Faction, loDiploScoreObj.Seller.Country) then
				loDiploScoreObj.Score = loDiploScoreObj.Score + 20
			end
		end
	end

	if loDiploScoreObj.Score > 0 then
		loDiploScoreObj.Score = Utils.CallGetScoreAI(loDiploScoreObj.Seller.Name, "DiploScore_LicenceTechnology", loDiploScoreObj)
	end
	
	return loDiploScoreObj.Score
end

-- #######################
-- Support Methods
-- #######################
function P.ProductionCheck(voType, voProductionData)
	local lbLicenseRequired = false
	
	if voType.Type ~= "Land" then
		local loSubUnit = CSubUnitDataBase.GetSubUnit(voType.Name)
		
		-- Try to License it if we do not have it
		if not(voProductionData.TechStatus:IsUnitAvailable(loSubUnit)) and not(loSubUnit:IsCapitalShip()) then
			lbLicenseRequired = true -- Set the License required flag
		
			local liUnitMPcost = loSubUnit:GetBuildCostMP():Get()
			local liTotalMP = voProductionData.UnitNeeds[voType.Index] * liUnitMPcost
			
			if voProductionData.ManpowerTotal < liTotalMP then
				voProductionData.UnitNeeds[voType.Index] = math.floor(voProductionData.ManpowerTotal / liUnitMPcost)
			end		
			
			-- WE do not do parralel runs just one serial run
			voProductionData.UnitNeeds[voType.Index] = math.min(voProductionData.UnitNeeds[voType.Index], voType.Serial)
			
			-- Performance check, make sure we have to build something after MP check
			if voProductionData.UnitNeeds[voType.Index] > 0 then
				-- Creater the Buyer Object
				local loBuyerInfo = {
					Country = voProductionData.ministerCountry,
					HasFaction = voProductionData.ministerCountry:HasFaction(),
					Faction = voProductionData.ministerCountry:GetFaction(),
					Ideology = tostring(voProductionData.ministerCountry:GetRulingIdeology():GetGroup():GetKey()),
					Pool = voProductionData.ministerCountry:GetPool():Get(CGoodsPool._MONEY_):Get()}
					
				local loLowestBidder = nil
				
				for loDiploStatus in loBuyerInfo.Country:GetDiplomacy() do
					if not(loDiploStatus:HasWar()) then
						local loSellerInfo = {
							Tag = loDiploStatus:GetTarget(),
							Country = nil,
							TechStatus = nil,
							SpamPenalty = nil,
							Faction = nil,
							Ideology = nil,
							Units = voProductionData.UnitNeeds[voType.Index],
							Command = nil,
							Score = 0,
							Cost = 0}

						loSellerInfo.Country = loSellerInfo.Tag:GetCountry()
						loSellerInfo.Faction = loSellerInfo.Country:GetFaction()
						loSellerInfo.Ideology = tostring(loSellerInfo.Country:GetRulingIdeology():GetGroup():GetKey())
						
						loSellerInfo.TechStatus = loSellerInfo.Country:GetTechnologyStatus()
						
						-- They can produce the tech so figure out cost
						if loSellerInfo.TechStatus:IsUnitAvailable(loSubUnit) then
							loSellerInfo.Faction = loSellerInfo.Country:GetFaction()
							loSellerInfo.Ideology = tostring(loSellerInfo.Country:GetRulingIdeology():GetGroup():GetKey())
							loSellerInfo.SpamPenalty = voProductionData.ministerAI:GetSpamPenalty(loSellerInfo.Tag)
							
							loSellerInfo.Cost = P.CalculateCost(loBuyerInfo, loSellerInfo, loSubUnit)
							
							-- Now figure out how many we can afford
							if loBuyerInfo.Pool < (loSellerInfo.Cost * loSellerInfo.Units) then
								loSellerInfo.Units = math.floor(loBuyerInfo.Pool / loSellerInfo.Cost)
							end
						
							-- We can afford some so lets get a score
							if loSellerInfo.Units > 0 then
								-- Create Command Object
								loSellerInfo.Command = CLicenceTechnologyAction(voProductionData.ministerTag, loSellerInfo.Tag)
								loSellerInfo.Command:SetSubunit(loSubUnit)
								loSellerInfo.Command:SetMoney(CFixedPoint(loSellerInfo.Cost))
								loSellerInfo.Command:SetSerial(loSellerInfo.Units)
								loSellerInfo.Command:SetParallel(1)
								
								loSellerInfo.Score = DiploScore_LicenceTechnology(voProductionData.ministerAI, voProductionData.ministerTag, loSellerInfo.Tag, nil, loSellerInfo.Command)
								
								if (loSellerInfo.Score - loSellerInfo.SpamPenalty) > 50 then
									if not(loLowestBidder) then
										loLowestBidder = loSellerInfo
									else
										-- Always go with the cheapest bidder
										if loLowestBidder.Cost > loSellerInfo.Cost then
											loLowestBidder = loSellerInfo
										elseif loLowestBidder.Cost == loSellerInfo.Cost then
											-- Cost is the same so go with the better score
											if loLowestBidder.Score < loSellerInfo.Score then
												loLowestBidder = loSellerInfo
											end
										end
									end
								end
							end
						end
					end
				end
				
				-- We found someone to license this from so ask
				--   Do not take into account the IC incase they say no this way que is always full
				if loLowestBidder then
					-- Update MP count on assumption we will build the unit
					voProductionData.ManpowerTotal = voProductionData.ManpowerTotal - loLowestBidder.Units * liUnitMPcost
					voProductionData.ministerAI:PostAction(loLowestBidder.Command)
				end
			end
		end
	end
	
	return lbLicenseRequired, voProductionData.ManpowerTotal
end

function P.CalculateCost(voBuyerInfo, voSellerInfo, voSubUnit)
	local liICCode = voSubUnit:GetBuildCostIC():Get()
	local liBuildTime = voSubUnit:GetBuildTime():Get()
	local liCost = 0

	if voBuyerInfo.Faction == voSellerInfo.Faction and voBuyerInfo.HasFaction then
		if voBuyerInfo.Country:IsAtWar() then
			liCost = (liICCode * liBuildTime) * 0.005 -- Half of 1 percent
		else
			liCost = (liICCode * liBuildTime) * 0.0075 -- 75% of 1 percent
		end
	elseif voBuyerInfo.Ideology == voSellerInfo.Ideology then
		liCost = (liICCode * liBuildTime) * 0.0085 -- 85% of 1 percent
	else
		liCost = (liICCode * liBuildTime) * 0.01 -- 1 percent
	end
	
	return liCost
end

-- ###############################################
-- END OF LICENSE Support methods
-- ###############################################

return Support_License