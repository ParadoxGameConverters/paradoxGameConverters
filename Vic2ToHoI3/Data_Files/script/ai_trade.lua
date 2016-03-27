local P = {}
Support_Trade = P


-- ###########################
-- Called by the EXE and handles the Analyzing of offered trades
-- ###########################
function DiploScore_OfferTrade(voAI, voFromTag, voToTag, voObserverTag, voTradeAction, voTradedFrom, voTradedTo)
	local loDiploScoreObj = {
		Score = 0,
		DefaultScore = 50,
		ministerAI = voAI,
		TradeRoute = voTradeAction:GetRoute(),
		TagName = nil,
		BuyerTag = nil,
		BuyerCountry = nil,
		BuyerContinent = nil,
		BuyerIdeology = nil,
		SellerTag = nil,
		SellerCountry = nil,
		SellerContinent = nil,
		SellerIdeology = nil,
		SellerStrategy = nil,
		Money = nil,
		IsNeighbor = nil,
		ResourceRequest = nil,
		Relation = nil,
		FreeTrade = false,
		NeedConvoy = false,
		HumanSelling = false,
		BuyerResources = nil,
		SellerResources = nil}
		
	loDiploScoreObj.Relation = loDiploScoreObj.ministerAI:GetRelation(voToTag, voFromTag)		
	loDiploScoreObj.FreeTrade = Support_Trade.FreeTradeCheck(voAI, voToTag, voFromTag, loDiploScoreObj.Relation)
	
	-- Two way trade get out!
	-- 0 cost trade but make sure they are not Commintern
	if (voTradedTo.vMoney > 0 and voTradedFrom.vMoney > 0)
	or (voTradedTo.vMoney == 0 and voTradedFrom.vMoney == 0 and not(loDiploScoreObj.FreeTrade)) then
		return 0
	end

	local liFromCount = 0
	local liToCount = 0
		
	-- They can trade freely between eachother
	if loDiploScoreObj.FreeTrade then
		liFromCount = voTradedFrom.vMetal + voTradedFrom.vEnergy + voTradedFrom.vRareMaterials + voTradedFrom.vCrudeOil + voTradedFrom.vSupplies + voTradedFrom.vFuel
		liToCount = voTradedTo.vMetal + voTradedTo.vEnergy + voTradedTo.vRareMaterials + voTradedTo.vCrudeOil + voTradedTo.vSupplies + voTradedTo.vFuel
	end

	if (voTradedTo.vMoney > 0)
	or (lbFreeTrader and liFromCount > 0 and liToCount <= 0) then
		-- Get the Money amount
		loDiploScoreObj.Money = voTradedTo.vMoney
		loDiploScoreObj.ResourceRequest = voTradedFrom
		
		loDiploScoreObj.BuyerTag = loDiploScoreObj.TradeRoute:GetTo()
		loDiploScoreObj.SellerTag = loDiploScoreObj.TradeRoute:GetFrom()
		loDiploScoreObj.BuyerCountry = loDiploScoreObj.BuyerTag:GetCountry()
		loDiploScoreObj.SellerCountry = loDiploScoreObj.SellerTag:GetCountry()
		
	else
		-- Get the Money amount
		loDiploScoreObj.Money = voTradedFrom.vMoney
		loDiploScoreObj.ResourceRequest = voTradedTo
		
		loDiploScoreObj.BuyerTag = loDiploScoreObj.TradeRoute:GetFrom()
		loDiploScoreObj.SellerTag = loDiploScoreObj.TradeRoute:GetTo()
		loDiploScoreObj.BuyerCountry = loDiploScoreObj.BuyerTag:GetCountry()
		loDiploScoreObj.SellerCountry = loDiploScoreObj.SellerTag:GetCountry()
	end		
		
	-- If the FromTag and SellerTag same person that means its a human selling
	if voFromTag == loDiploScoreObj.SellerTag then
		loDiploScoreObj.HumanSelling = true
	end
		
	loDiploScoreObj.NeedConvoy = loDiploScoreObj.BuyerCountry:NeedConvoyToTradeWith(loDiploScoreObj.SellerTag)
	
	-- Do we need transports for this?
	if loDiploScoreObj.NeedConvoy then
		if loDiploScoreObj.BuyerCountry:GetTransports() == 0 then
			return 0
		end
	end		

	loDiploScoreObj.BuyerResources = Support_Trade.Trade_GetResources(loDiploScoreObj.BuyerTag, loDiploScoreObj.BuyerCountry)
	loDiploScoreObj.SellerResources = Support_Trade.Trade_GetResources(loDiploScoreObj.SellerTag, loDiploScoreObj.SellerCountry, loDiploScoreObj.HumanSelling)

	local lbFoundOne = false
	for k, v in pairs(loDiploScoreObj.BuyerResources) do
		if not(v.ByPass) then
			if loDiploScoreObj.ResourceRequest[v.TradeOBJ] > 0 then
				if lbFoundOne then
					return 0 -- We only process one resource at a time
				end
				if loDiploScoreObj.HumanSelling then
					if loDiploScoreObj.ResourceRequest[v.TradeOBJ] <= v.Buy and v.TradeAway <= 0 then
						if loDiploScoreObj.Money <= loDiploScoreObj.BuyerResources.MONEY.CanSpend then
							loDiploScoreObj.Score = (((loDiploScoreObj.ResourceRequest[v.TradeOBJ] * v.ScoreFactor) * v.ShortPercentage) + loDiploScoreObj.DefaultScore)
						end
					end
				else
					if loDiploScoreObj.ResourceRequest[v.TradeOBJ] <= loDiploScoreObj.SellerResources[k].Sell then
						loDiploScoreObj.Score = (((loDiploScoreObj.ResourceRequest[v.TradeOBJ] * v.ScoreFactor) * v.ShortPercentage) + loDiploScoreObj.DefaultScore)
					end
				end
				
				lbFoundOne = true
			end
		end
	end
		
	-- Now shift the score based on Diplomatic relations!
	if loDiploScoreObj.Score > 0 then
		-- Political checks
		loDiploScoreObj.SellerStrategy = loDiploScoreObj.SellerCountry:GetStrategy()
		
		loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.SellerStrategy:GetAntagonism(loDiploScoreObj.BuyerTag) / 15			
		loDiploScoreObj.Score = loDiploScoreObj.Score + loDiploScoreObj.SellerStrategy:GetFriendliness(loDiploScoreObj.BuyerTag) / 10
		loDiploScoreObj.Score = loDiploScoreObj.Score - loDiploScoreObj.Relation:GetThreat():Get() / 5
		loDiploScoreObj.Score = loDiploScoreObj.Score + tonumber(tostring(loDiploScoreObj.Relation:GetValue():GetTruncated())) / 3
		
		if loDiploScoreObj.Relation:IsGuaranteed() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		if loDiploScoreObj.Relation:HasFriendlyAgreement() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		if loDiploScoreObj.Relation:IsFightingWarTogether() then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 15
		end

		loDiploScoreObj.IsNeighbor = loDiploScoreObj.BuyerCountry:IsNonExileNeighbour(loDiploScoreObj.SellerTag)	
		loDiploScoreObj.SellerContinent = loDiploScoreObj.SellerCountry:GetActingCapitalLocation():GetContinent()
		loDiploScoreObj.BuyerContinent = loDiploScoreObj.BuyerCountry:GetActingCapitalLocation():GetContinent()
		
		if loDiploScoreObj.IsNeighbor then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 15
		end
		
		if loDiploScoreObj.SellerContinent == loDiploScoreObj.BuyerContinent then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		
		loDiploScoreObj.BuyerIdeology = tostring(loDiploScoreObj.BuyerCountry:GetRulingIdeology():GetGroup():GetKey())
		loDiploScoreObj.SellerIdeology = tostring(loDiploScoreObj.SellerCountry:GetRulingIdeology():GetGroup():GetKey())
		
		if loDiploScoreObj.BuyerIdeology == loDiploScoreObj.SellerIdeology then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 10
		end
		
		if Support.IsFriend(loDiploScoreObj.ministerAI, loDiploScoreObj.BuyerCountry:GetFaction(), loDiploScoreObj.SellerCountry) then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
		
		-- Land Route gets bigger bonus
		if not(loDiploScoreObj.NeedConvoy) then
			loDiploScoreObj.Score = loDiploScoreObj.Score + 5
		end
	end
	
	if loDiploScoreObj.Score > 0 then
		loDiploScoreObj.TagName = tostring(voFromTag)
		loDiploScoreObj.Score = Utils.CallGetScoreAI(voToTag, "DiploScore_OfferTrade", loDiploScoreObj)
	end

	return loDiploScoreObj.Score
end
function EvalutateExistingTrades(voAI, ministerTag)
	local CTradeData = {
		ministerAI = voAI,
		ministerTag = ministerTag,
		ministerCountry = ministerTag:GetCountry(),
		Resources = nil}
	
	CTradeData.Resources = Support_Trade.Trade_GetResources(CTradeData.ministerTag, CTradeData.ministerCountry)
	
	local laHighResource = {}
	local laShortResource = {}
	local laCancel = {}
	local lbContinue = false
	local lbBuying = false
	local liMoneyOverage = 0
	
	-- Figure out if we have a glutten of resources coming in
	for k, v in pairs(CTradeData.Resources) do
		if not(v.Bypass) then
			if v.Buy > 0 then
				lbBuying = true
			end

			-- We are buying and selling the same resource
			if v.TradeFor > 0 and v.TradeAway > 0 then
				-- Cancel something
				if v.DailyBalance > v.Buffer then
					laShortResource[k] = true
					lbContinue = true
				else
					laShortResource[k] = true
					lbContinue = true
				end
			else
				if k == "SUPPLIES" or k == "CRUDE_OIL" then
					-- If we are loosing money (give the buffer some leasticity before canceling by cutting it in half
					if CTradeData.Resources.MONEY.DailyBalance < (CTradeData.Resources.MONEY.Buffer * 0.5)
					or (v.Pool > v.BufferCancelCap) then
						if v.TradeFor > 0 then
							laHighResource[k] = true
							lbContinue = true
						end
					end
				else
					-- Gluten Check
					if v.TradeFor > 0 and v.Buy <= 0 then
						if (v.DailyBalance > v.Buffer) then
							-- If we are higher than our cancel pool or our daily balance is greater than the buffer multiplied by 2 (give some elasticity)
							if (v.Pool > v.BufferCancelCap) or (v.DailyBalance > (v.Buffer * 1.5)) then
								laHighResource[k] = true
								lbContinue = true
							end
						end
						
					-- Selling what we need check
					elseif v.TradeAway > 0 then
						if v.DailyBalance < (v.Buffer * 0.5) then
							laShortResource[k] = true
							lbContinue = true
						end
					end
				end
			end
		end
	end
	
	-- Few extra checks in case we have to much or to little money
	if not(lbContinue) then
		-- We are loosing money so look for stuff to cancel
		if CTradeData.Resources.MONEY.DailyBalance < (CTradeData.Resources.MONEY.Buffer * 0.5) then
			if CTradeData.Resources.FUEL.TradeFor > 0 then
				laHighResource["FUEL"] = true
				lbContinue = true
			end
		end	
	
		-- Supply check to see if we should cancel cause we are buying to much
		if CTradeData.Resources.MONEY.DailyBalance > (CTradeData.Resources.MONEY.Buffer * 1.5) then
			if CTradeData.Resources.SUPPLIES.TradeFor > 0 then
				if CTradeData.Resources.SUPPLIES.DailyBalance > (CTradeData.Resources.SUPPLIES.Buffer * 1.5) then
					if CTradeData.Resources.SUPPLIES.DailyBalance > (CTradeData.Resources.MONEY.Buffer) then
						laHighResource["SUPPLIES"] = true
						lbContinue = true
					end
				end
				
			-- Check to see if we are trading away to much
			elseif CTradeData.Resources.SUPPLIES.TradeAway > 0 and not(lbBuying) then
				liMoneyOverage = CTradeData.Resources.MONEY.DailyBalance - CTradeData.Resources.MONEY.Buffer
				
				if liMoneyOverage > 0 then
					laShortResource["SUPPLIES"] = true
					lbContinue = true
				end
			end
		end
	end

	for loTradeRoute in CTradeData.ministerCountry:AIGetTradeRoutes() do
		-- Kill Inactive Trades
		if loTradeRoute:IsInactive() and CTradeData.ministerAI:HasTradeGoneStale(loTradeRoute) then
			local loCountryTag= loTradeRoute:GetFrom()
			
			if loCountryTag == CTradeData.ministerTag then
				loCountryTag = loTradeRoute:GetTo()
			end
			
			local loTradeAction = CTradeAction(CTradeData.ministerTag, loCountryTag)
			loTradeAction:SetRoute(loTradeRoute)
			loTradeAction:SetValue(false)
			
			if loTradeAction:IsSelectable() then
				CTradeData.ministerAI:PostAction(loTradeAction)
			end
			
		else
			-- If nothing to do skip this
			if lbContinue then
				local loCountryTag = loTradeRoute:GetFrom()
				
				if loCountryTag == CTradeData.ministerTag then
					loCountryTag = loTradeRoute:GetTo()
				end
				
				for k, v in pairs(CTradeData.Resources) do
					if not(v.Bypass) then
						local Trade = {
							Trade = loTradeRoute,
							Command = nil,
							Money = 0,
							Quantity = 0}

						-- Are we short or High on anything
						if laShortResource[k] or laHighResource[k] then
							if laShortResource[k] then
								Trade.Quantity = loTradeRoute:GetTradedFromOf(v.CGoodsPool):Get()
							elseif laHighResource[k] then
								Trade.Quantity = loTradeRoute:GetTradedToOf(v.CGoodsPool):Get()
							end

							local GetTradedFromOf = loTradeRoute:GetTradedFromOf(v.CGoodsPool):Get()
							local GetTradedToOf = loTradeRoute:GetTradedToOf(v.CGoodsPool):Get()
							
							-- Look for the lowest one to cancel
							if Trade.Quantity > 0 then
								if k == "SUPPLIES" and liMoneyOverage > 0 then
									Trade.Money = loTradeRoute:GetTradedToOf(CTradeData.Resources.MONEY.CGoodsPool):Get()
									
									-- Clean up our Over selling of supplies
									if Trade.Money <= liMoneyOverage and not(laCancel[k]) then
										if not(laCancel[k]) then
											Trade.Command = CTradeAction(CTradeData.ministerTag, loCountryTag)
											laCancel[k] = Trade
										else
											if laCancel[k].Money < Trade.Money then
												Trade.Command = CTradeAction(CTradeData.ministerTag, loCountryTag)
												laCancel[k] = Trade
											end
										end										
									end
								elseif not(laCancel[k]) then
									Trade.Command = CTradeAction(CTradeData.ministerTag, loCountryTag)
									laCancel[k] = Trade
								else
									-- Regular resource check
									if laCancel[k].Quantity > Trade.Quantity then
										Trade.Command = CTradeAction(CTradeData.ministerTag, loCountryTag)
										laCancel[k] = Trade
									end
								end
							end
						end
					end
				end
			end
		end
	end

	for k, v in pairs(laCancel) do
		v.Command:SetRoute(v.Trade)
		v.Command:SetValue(false)
		CTradeData.ministerAI:PostAction(v.Command)
	end
end
function ProposeTrades(vAI, ministerTag)
	local TradeData = {
		ministerAI = vAI,
		ministerTag = ministerTag,
		ministerCountry = ministerTag:GetCountry(),
		Resources = nil}
		
	TradeData.Resources = Support_Trade.Trade_GetResources(TradeData.ministerTag, TradeData.ministerCountry)
	
	local laTrades = {} -- Contains an array of trades we will try to execute
	local lbNeedTrades = false -- DO we need any actual Trades
	local liMinTradeAmount = 0.25 -- Another copy if this in Support_Trade.Trade_GetResources
	local liMaxTradeAmount = 50
	
	for k, v in pairs(TradeData.Resources) do
		-- Can We solve our problems by Canceling Trades
	--	if v.Buy > 0 and v.TradeAway > 0 then
	--		v.Buy = TradeData.ministerAI:EvaluateCancelTrades(v.Buy, v.CGoodsPool)
	--	end
		
		if v.Buy > 0 or v.Sell > 0 then 
			lbNeedTrades = true
		end
	end
	
	-- Performance check, skip if we have nothing to do
	if lbNeedTrades then
		for loTCountry in CCurrentGameState.GetCountries() do
			
			local loCountryTrade = {
				Tag = loTCountry:GetCountryTag(),
				Country = loTCountry,
				Resources = nil,
				SpamPenalty = nil,
				FreeTrade = false,
				Relation = nil}

			if loCountryTrade.Tag ~= TradeData.ministerTag then
				if not(TradeData.ministerCountry:HasDiplomatEnroute(loCountryTrade.Tag)) and loTCountry:Exists() then
					loCountryTrade.Relation = TradeData.ministerAI:GetRelation(TradeData.ministerTag, loCountryTrade.Tag)
					
					if P.Can_Click_Button(loCountryTrade, TradeData) then
						loCountryTrade.Resources = Support_Trade.Trade_GetResources(loCountryTrade.Tag, loCountryTrade.Country)
						loCountryTrade.SpamPenalty = TradeData.ministerAI:GetSpamPenalty(loCountryTrade.Tag)
						loCountryTrade.FreeTrade = Support_Trade.FreeTradeCheck(TradeData.ministerAI, loCountryTrade.Tag, TradeData.ministerTag, loCountryTrade.Relation)
						
						for k, v in pairs(loCountryTrade.Resources) do
							if not(v.Bypass) then
								local loTrade = {
									Resource = v.CGoodsPool,
									Score = 0,
									Buy = false,
									Sell = false,
									FreeTrade = loCountryTrade.FreeTrade,
									Command = nil,
									Quantity = 0}
								
								-- They have something we need
								if v.Sell > 0 and TradeData.Resources[k].Buy > 0 then
									loTrade.Buy = true
									loTrade.Quantity = math.min(v.Sell, TradeData.Resources[k].Buy, liMaxTradeAmount)
								else
									if loCountryTrade.Resources.MONEY.DailyIncome > 0 or loCountryTrade.FreeTrade then
										-- They need something we are selling
										if v.Buy > 0 and TradeData.Resources[k].Sell > 0 then
											loTrade.Sell = true
											loTrade.Quantity = math.min(v.Buy, TradeData.Resources[k].Sell, liMaxTradeAmount)
										end
									end
								end
								
								-- Now lets gets a score
								if loTrade.Buy then
									local loCommand = CTradeAction(TradeData.ministerTag, loCountryTrade.Tag)
									loCommand:SetTrading(CFixedPoint(loTrade.Quantity), v.CGoodsPool)
									
									if not(TradeData.ministerAI:AlreadyTradingDisabledResource(loCommand:GetRoute())) then
										if loCommand:IsValid() and loCommand:IsSelectable() then
											local liCost = loCommand:GetTrading(CGoodsPool._MONEY_, TradeData.ministerTag):Get()

											if liCost > TradeData.Resources.MONEY.CanSpend and not(loCountryTrade.FreeTrade) then
												local liMultiplier = liCost / loTrade.Quantity
												loTrade.Quantity = TradeData.Resources.MONEY.CanSpend / liMultiplier
												loCommand:SetTrading(CFixedPoint(loTrade.Quantity), v.CGoodsPool)
											end
											
											if loTrade.Quantity > liMinTradeAmount then
												loTrade.Score = loCommand:GetAIAcceptance() - loCountryTrade.SpamPenalty
												
												if loTrade.Score > 50 then
													loTrade.Command = loCommand
												end
											end
										end
									end
									
								elseif loTrade.Sell then
									local loCommand = CTradeAction(TradeData.ministerTag, loCountryTrade.Tag)
									loCommand:SetTrading(CFixedPoint(loTrade.Quantity * -1), v.CGoodsPool)
									
									if not(TradeData.ministerAI:AlreadyTradingDisabledResource(loCommand:GetRoute())) then
										if loCommand:IsValid() and loCommand:IsSelectable() then
											local liCost = loCommand:GetTrading(CGoodsPool._MONEY_, loCountryTrade.Tag):Get()

											if liCost > loCountryTrade.Resources.MONEY.CanSpend and not(loCountryTrade.FreeTrade) then
												local liMultiplier = liCost / loTrade.Quantity
												loTrade.Quantity = loCountryTrade.Resources.MONEY.CanSpend / liMultiplier
												loCommand:SetTrading(CFixedPoint(loTrade.Quantity * -1), v.CGoodsPool)
											end

											if loTrade.Quantity > liMinTradeAmount then
												loTrade.Score = loCommand:GetAIAcceptance() - loCountryTrade.SpamPenalty

												if loTrade.Score > 50 then
													loTrade.Command = loCommand
												end
											end
										end
									end
								end
								
								-- Add it to the processing Array
								if loTrade.Command then
									laTrades[tostring(loCountryTrade.Tag) .. tostring(k)] = loTrade
								end
							end
						end
					end
				end
			end
		end
		
		local loFinalTrade = nil
		-- Do we have potential trades to process
		for k, v in pairs(laTrades) do
			if not(loFinalTrade) then
				loFinalTrade = v
			else
				-- Free Trades get priority
				if v.FreeTrade then
					if not(loFinalTrade.FreeTrade) then
						loFinalTrade = v
					elseif v.Score > loFinalTrade.Score then
						loFinalTrade = v
					end
				elseif v.Score > loFinalTrade.Score then
					loFinalTrade = v
				end
			end
		end
		
		if loFinalTrade then
			TradeData.ministerAI:PostAction(loFinalTrade.Command)
		end
	end
end

-- #######################
-- Support Methods
-- #######################
function P.Trade_GetResources(voTag, voCountry, vbHumanSelling)
	local liMinTradeAmount = 0.25
	local ResourceData = {
		ministerTag = voTag,
		ministerCountry = voCountry,
		TechStatus = nil,
		BaseIC = nil,
		TotalIC = nil,
		ModifierICTech = nil,
		ModifierICGlobal = nil}

	local laResouces = {
		MONEY = {
			ByPass = true,
			Buffer = 0.1,
			BufferSaleCap = 20, 
			TradeOBJ = "vMoney",
			CGoodsPool = CGoodsPool._MONEY_},
		METAL = {
			Buffer = 1, 			-- Amount extra to keep abouve our needs
			BufferSaleCap = 300, 	-- Amount we need in reserve before we sell the resource
			BufferBuyCap = 80000, 	-- Amount we need before we stop actively buying (existing trades are NOT cancelled)
			BufferCancelCap = 90000, -- Amount we need before we cancel trades simply because we have to much
			ScoreFactor = 2, 		-- Multiplier used when calculating the final trade score against the resource (resource count * ScoreFactor)
			Multiplier = 1, 		-- (DO NOT OVERIDE THIS IN COUNTRY FILES)
			TradeOBJ = "vMetal",	-- (DO NOT OVERIDE THIS IN COUNTRY FILES)
			CGoodsPool = CGoodsPool._METAL_},	-- (DO NOT OVERIDE THIS IN COUNTRY FILES)
		ENERGY = {
			Buffer = 5,
			BufferSaleCap = 600,
			BufferBuyCap = 80000,
			BufferCancelCap = 90000,
			ScoreFactor = 1,
			Multiplier = 2, 		-- Factor to use to calculate needs
			TradeOBJ = "vEnergy",
			CGoodsPool = CGoodsPool._ENERGY_},
		RARE_MATERIALS = {
			Buffer = 0.5,
			BufferSaleCap = 150,
			BufferBuyCap = 80000,
			BufferCancelCap = 90000,
			ScoreFactor = 4,
			Multiplier = 0.5, 		-- Factor to use to calculate needs
			TradeOBJ = "vRareMaterials",
			CGoodsPool = CGoodsPool._RARE_MATERIALS_},
		CRUDE_OIL = {
			BuyOveride = true,
			Buffer = 0.25,
			BufferSaleCap = 500,
			BufferBuyCap = 80000,
			BufferCancelCap = 90000,
			ScoreFactor = 2,
			TradeOBJ = "vCrudeOil",
			CGoodsPool = CGoodsPool._CRUDE_OIL_},
		SUPPLIES = {
			BuyOveride = true,
			Buffer = 1,
			BufferSaleCap = 5000, -- Ignored for supplies
			BufferBuyCap = 80000,
			BufferCancelCap = 90000,
			ScoreFactor = 3,
			TradeOBJ = "vSupplies",
			CGoodsPool = CGoodsPool._SUPPLIES_},
		FUEL = {
			BuyOveride = true,
			Buffer = 0.25,
			BufferSaleCap = 500,
			BufferBuyCap = 80000,
			BufferCancelCap = 90000,
			ScoreFactor = 2,
			TradeOBJ = "vFuel",
			CGoodsPool = CGoodsPool._FUEL_}}

	-- Need to do this formula because if any resource falls to O then GetTotalIC does not return their true max
	ResourceData.BaseIC = ResourceData.ministerCountry:GetMaxIC()
	ResourceData.TechStatus = ResourceData.ministerCountry:GetTechnologyStatus()
	ResourceData.ModifierICTech = ResourceData.TechStatus:GetIcModifier():Get()
	ResourceData.ModifierICGlobal = ResourceData.ministerCountry:GetGlobalModifier():GetValue(CModifier._MODIFIER_GLOBAL_IC_):Get()
	ResourceData.TotalIC = math.floor(ResourceData.BaseIC + (ResourceData.BaseIC * ResourceData.ModifierICTech) + (ResourceData.BaseIC * ResourceData.ModifierICGlobal))

	local loFunRef = Utils.HasCountryAIFunction(ResourceData.ministerTag, "TradeWeights")
	
	if loFunRef then
		local laResourcePassed = loFunRef(ResourceData)
		
		if laResourcePassed then
			for k, v in pairs(laResourcePassed) do
				for x, y in pairs(laResourcePassed[k]) do
					laResouces[k][x] = laResourcePassed[k][x]
				end
			end
		end
	end
			
	local lbBuying = false
	local loResource = CResourceValues()
	local loTradeAway = ResourceData.ministerCountry:GetTradedAwaySansAlliedSupply()
	local loTradeFor = ResourceData.ministerCountry:GetTradedForSansAlliedSupply()
	

	for k, v in pairs(laResouces) do
		loResource:GetResourceValues(ResourceData.ministerCountry, v.CGoodsPool)
		v.DailyBalance = loResource.vDailyBalance
		v.DailyExpense = loResource.vDailyExpense -- Modify this to use IC to count expense not resources
		v.DailyIncome = loResource.vDailyIncome
		v.DailyHome = Support.CalculateHomeProduced(loResource)
		v.Pool = loResource.vPool
		
		-- If we have a multiplier then check
		if v.Multiplier then
			-- This is to handle in case resources hit 0 so the AI knows it needs allot more
			v.DailyExpense = math.max(v.DailyExpense, (v.Multiplier * ResourceData.TotalIC))
			v.DailyBalance = v.DailyIncome - v.DailyExpense
		end
		
		v.Buy = 0
		v.Sell = 0
		v.TradeAway = 0
		v.TradeFor = 0
		v.ShortPercentage = 0
		
		-- Now Calculate what our needs are
		if not(v.ByPass) then
			v.TradeFor = loTradeFor:GetFloat(v.CGoodsPool)
			v.TradeAway = loTradeAway:GetFloat(v.CGoodsPool)
			
			if v.DailyBalance < v.Buffer and v.TradeAway <= 0 then
				-- Make sure we are not being a gluten with the resources before we buy
				if v.Pool < v.BufferBuyCap then
					local liBuy = (v.DailyBalance  * -1) + v.Buffer
					if liBuy >= liMinTradeAmount then
						v.Buy = liBuy
						
						if not(v.BuyOveride) then
							lbBuying = true
						end
					end
				end
			elseif v.DailyBalance > v.Buffer then
				if (v.TradeFor <= 0 or vbHumanSelling) and (v.Pool > v.BufferSaleCap) then
					v.Sell = v.DailyBalance - v.Buffer
				end
			end
		end
		
		-- Calculate Short Percentage
		if v.Buy > 0 and v.DailyIncome > 0 and v.DailyExpense > 0 then
			v.ShortPercentage = (1.0 - (v.DailyIncome / v.DailyExpense))
		elseif v.Buy > 0 then
			v.ShortPercentage = 1.0
		end
	end
	
	laResouces.MONEY.CanSpend = laResouces.MONEY.DailyBalance - laResouces.MONEY.Buffer
	
	-- Crude oil needs check, makes sure we don't buy crude and instead spend cash on supplies
	if laResouces.CRUDE_OIL.Buy > 0 then
		if (laResouces.FUEL.DailyBalance > laResouces.FUEL.Buffer
		and laResouces.CRUDE_OIL.DailyHome > (laResouces.FUEL.DailyBalance * 0.5)
		and laResouces.FUEL.Pool > laResouces.FUEL.BufferSaleCap)
		or lbBuying then
			laResouces.CRUDE_OIL.Buy = 0
		end
	end
	
	-- We are buying but short on money so setup Supply selling
	if (laResouces.MONEY.DailyBalance <= laResouces.MONEY.Buffer and laResouces.SUPPLIES.TradeFor <= 0) then
		local liTotalIC = ResourceData.ministerCountry:GetTotalIC()
		laResouces.SUPPLIES.Buy = 0
		laResouces.SUPPLIES.Sell = math.min(20, (liTotalIC - laResouces.SUPPLIES.TradeAway))
		laResouces.SUPPLIES.ShortPercentage = 1.0
		
	-- We are not buying and have money to spend to pick up supplies
	elseif not(lbBuying) and laResouces.MONEY.DailyBalance > laResouces.MONEY.Buffer and laResouces.SUPPLIES.TradeAway <= 0 then 
		laResouces.SUPPLIES.Sell = 0
		laResouces.SUPPLIES.Buy = math.min(20, ((laResouces.SUPPLIES.DailyExpense * 1.2) - laResouces.SUPPLIES.TradeFor))
		laResouces.SUPPLIES.ShortPercentage = 1.0
		
	-- Not buying or selling supplies so set it to 0
	else
		laResouces.SUPPLIES.Buy = 0
		laResouces.SUPPLIES.Sell = 0
	end	
	
	return laResouces
end
function P.FreeTradeCheck(voAI, voToTag, voFromTag, voRelation)
	-- Commiterm Check or ALlow Debt check
	if voAI:CanTradeFreeResources(voToTag, voFromTag) or voRelation:AllowDebts() then
		return true
	else
		return false
	end
end
function P.Can_Click_Button(voTarget, voTradeData)
	if voTarget.Country:Exists() then
		if voTarget.Tag:IsReal() then
			if voTarget.Tag:IsValid() then
				if not(voTarget.Country:IsPuppet()) then
					if not(voTarget.Relation:HasWar()) then
						if not(voTradeData.ministerCountry:HasDiplomatEnroute(voTarget.Tag)) then
							return true
						end
					end
				end
			end
		end
	end
	
	return false
end
-- ###############################################
-- END OF Support methods
-- ###############################################

return Support_Embargo