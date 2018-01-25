
-- ######################
-- Global Variables
--    CAREFUL: Variables are remembered from country to country
-- ######################
local ProductionData = {} -- Gets reset each time the tick starts
-- When adding new ones the main arrays in the Production object need to be updated

--	militia_brigade = { 					#### Brigade now as specified in the units folder
--		Index = 3, 							#### Location of where the item is stored in the UnitNeeds, Counts etc.... arrays
--		Serial = 4, 						#### How long should the serial run be (default 1)
--		Size = 3, 							#### How many of this brigade type should be used when building the division (default 1)
--		Support = 1, 						#### How many support brigades to attach to the unit (default 0)
-- 		SecondaryMain = "motorized_brigade" #### Add one unit of this type to the division always
--		SupportGroup = "Infantry", 			#### What support group to use Look at SupportType for groups
--		Type = "Land", 						#### What is the main type of the unit (Land, Naval, Air)
--		SubType = "Militia", 				#### What is the Subtype of the unit
--		SupportType = {						#### The SupportGroup that the support unit can be used with
--			"Garrison",
--			"Militia",
--			"Marine",
--			"Infantry",
--			"Motor",
--			"Armor"},
--		SubUnit = "cag",					#### Secondary unit that needs to be built for this primary unit 
--		SubQuantity = 1},                   #### Quantity of the secondary unit that needs to be built for this primary unit

local UnitTypes = {
	-- Land Units
	hq_brigade = {
		Index = 1,
		Type = "Land",
		SubType = "Headquarters"},
	garrison_brigade = {
		Index = 2,
		Serial = 4,
		Size = 2,
		Support = 1,
		SupportGroup = "Garrison",
		Type = "Land",
		SubType = "Infantry"},
	militia_brigade = {
		Index = 3,
		Serial = 4,
		Size = 2,
		Support = 2,
		SupportGroup = "Militia",
		Type = "Land",
		SubType = "Infantry"},
	infantry_brigade = {
		Index = 4,
		Serial = 4,
		Size = 3,
		Support = 1,
		--SecondaryMain = "anti_tank_brigade",
		SupportGroup = "Infantry",
		Type = "Land",
		SubType = "Infantry"},
	cavalry_brigade = {
		Index = 5,
		Serial = 2,
		Size = 2,
		Type = "Land",
		SubType = "Cavalry"},
	bergsjaeger_brigade = {
		Index = 6,
		Serial = 3,
		Size = 3,
		Support = 1,
		SupportGroup = "Mountain",
		Type = "Land",
		SubType = "Special Forces"},
	marine_brigade = {
		Index = 7,
		Serial = 3,
		Size = 3,
		Support = 1,
		SupportGroup = "Marine",
		Type = "Land",
		SubType = "Special Forces"},
	paratrooper_brigade = {
		Index = 8,
		Serial = 3,
		Size = 3,
		CanPara = true,
		Type = "Land",
		SubType = "Special Forces"},
	partisan_brigade = {
		Index = 9,
		Type = "Land",
		SubType = "Partisan"},
	light_armor_brigade = {
		Index = 10,
		Serial = 2,
		Size = 2,
		Support = 2,
		SecondaryMain = "motorized_brigade",
		SupportGroup = "Armor",
		Type = "Land",
		SubType = "Armor"},
	armor_brigade = {
		Index = 11,
		Serial = 2,
		Size = 2,
		Support = 2,
		SecondaryMain = "motorized_brigade",
		SupportGroup = "Armor",
		Type = "Land",
		SubType = "Armor"},
	heavy_armor_brigade = {
		Index = 12,
		Serial = 2,
		Size = 1,
		Support = 2,
		SecondaryMain = "motorized_brigade",
		SupportGroup = "Armor",
		Type = "Land",
		SubType = "Heavy Armor"},
	mechanized_brigade = {
		Index = 13,
		Serial = 2,
		Size = 1,
		Support = 2,
		SecondaryMain = "motorized_brigade",
		SupportGroup = "Motor",
		Type = "Land",
		SubType = "Mech"},
	motorized_brigade = {
		Index = 14,
		Serial = 2,
		Size = 2,
		Support = 2,
		SupportGroup = "Motor",
		Type = "Land",
		SubType = "Motor"},

	-- Support Brigades
	anti_air_brigade = {
		Index = 15,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Infantry",
			"Garrison",
			"Militia"}},
	anti_tank_brigade = {
		Index = 16,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Militia"}},
	artillery_brigade = {
		Index = 17,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Infantry",
			"Militia"}},
	engineer_brigade = {
		Index = 18,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Mountain",
			"Marine",
			"Infantry",
			"Motor",
			"Armor"}},
	rocket_artillery_brigade = {
		Index = 19,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Infantry",
			"Militia"}},
	police_brigade = {
		Index = 20,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Garrison"}},
	armored_car_brigade = {
		Index = 21,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Infantry",
			"Motor",
			"Armor"}},
	sp_artillery_brigade = {
		Index = 22,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Motor",
			"Armor"}},
	sp_rct_artillery_brigade = {
		Index = 23,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Motor",
			"Armor"}},
	tank_destroyer_brigade = {
		Index = 24,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Motor",
			"Armor"}},
	mot_aa_brigade = {
		Index = 25,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {
			"Motor",
			"Armor"}},
	super_heavy_armor_brigade = {
		Index = 26,
		Type = "Land",
		SubType = "Support",
		SupportType = Utils.Set {"Infantry"}},
		
	-- Naval Units
	battlecruiser = {
		Index = 27,
		Type = "Naval",
		SubType = "Capital Ship"},
	battleship = {
		Index = 28,
		Type = "Naval",
		SubType = "Capital Ship"},
	super_heavy_battleship = {
		Index = 29,
		Type = "Naval",
		SubType = "Capital Ship"},
	carrier = {
		Index = 30,
		Type = "Naval",
		SubType = "Carrier",
		SubUnit = "cag",
		SubQuantity = 2},
	escort_carrier = {
		Index = 31,
		Serial = 2,
		Type = "Naval",
		SubType = "Carrier",
		SubUnit = "cag",
		SubQuantity = 1},
	cag = {
		Index = 32,
		Type = "Naval",
		SubType = "Carrier Plane"},
	destroyer = {
		Index = 33,
		Serial = 2,
		Type = "Naval",
		SubType = "Escort"},
	light_cruiser = {
		Index = 34,
		Type = "Naval",
		SubType = "Cruiser"},
	heavy_cruiser = {
		Index = 35,
		Type = "Naval",
		SubType = "Cruiser"},
	submarine = {
		Index = 36,
		Type = "Naval",
		SubType = "Submarine"},
	nuclear_submarine = {
		Index = 37,
		Type = "Naval",
		SubType = "Submarine"},

	-- Transport Craft (The Order matters as the last one available via tech is what the AI will build
	transport_ship = {
		Index = 38,
		Serial = 3,
		Type = "Naval",
		SubType = "Transport"},
		
	-- Invasion Craft (The Order matters as the last one available via tech is what the AI will build
	invasion_transport_ship = {
		Index = 39,
		Serial = 3,
		Type = "Naval",
		SubType = "Invasion"},
	assault_ship = {
		Index = 40,
		Serial = 3,
		Type = "Naval",
		SubType = "Invasion"},
		
	-- Air Units
	cas = {
		Index = 41,
		Type = "Air",
		SubType = "Ground"},
	interceptor = {
		Index = 42,
		Serial = 2,
		Type = "Air",
		SubType = "Fighter"},
	multi_role = {
		Index = 43,
		Serial = 2,
		Type = "Air",
		SubType = "Fighter"},
	rocket_interceptor = {
		Index = 44,
		Serial = 2,
		Type = "Air",
		SubType = "Fighter"},
	naval_bomber = {
		Index = 45,
		Serial = 2,
		Type = "Air",
		SubType = "Naval"},
	strategic_bomber = {
		Index = 46,
		Serial = 2,
		Type = "Air",
		SubType = "Strategic"},
	tactical_bomber = {
		Index = 47,
		Serial = 2,
		Type = "Air",
		SubType = "Tactical"},
	transport_plane = {
		Index = 48,
		Serial = 2,
		Type = "Air",
		SubType = "Transport Plane"},

	-- Secret Weapon
	flying_bomb = {
		Index = 49,
		Serial = 2,
		Type = "Secret",
		SubType = "Bomb"},
	flying_rocket = {
		Index = 50,
		Serial = 2,
		Type = "Secret",
		SubType = "Rocket"},
		
	-- Elite Units
	gurkha_brigade = {
		Index = 51,
		Serial = 4,
		Size = 3,
		CanPara = true,
		Type = "Land",
		SubType = "Elite Infantry"},
	waffen_brigade = {
		Index = 52,
		Serial = 4,
		Size = 2,
		Support = 2,
		SupportGroup = "Motor",
		Type = "Land",
		SubType = "Elite Motor"},
	ranger_brigade = {
		Index = 53,
		Serial = 4,
		Size = 3,
		CanPara = true,
		Type = "Land",
		SubType = "Elite Infantry"},
	imperial_brigade = {
		Index = 54,
		Serial = 4,
		Size = 3,
		Support = 1,
		SupportGroup = "Marine",
		Type = "Land",
		SubType = "Elite Infantry"},
	guards_brigade = {
		Index = 55,
		Serial = 4,
		Size = 2,
		Support = 1,
		SecondaryMain = "anti_tank_brigade",
		SupportGroup = "Infantry",
		Type = "Land",
		SubType = "Elite Infantry"},
	alpins_brigade = {
		Index = 56,
		Serial = 4,
		Size = 3,
		Support = 1,
		SupportGroup = "Mountain",
		Type = "Land",
		SubType = "Elite Infantry"},
	alpini_brigade = {
		Index = 57,
		Serial = 3,
		Size = 3,
		Support = 1,
		SupportGroup = "Mountain",
		Type = "Land",
		SubType = "Elite Infantry"}
}
	
-- ###################################
-- # Main Method called by the EXE
-- #####################################
function BalanceProductionSliders(ai, ministerCountry, prioSelection,
                                  vLendLease, vConsumer, vProduction, vSupply, vReinforce, vUpgrade, bHasReinforceBonus)
	local liOrigPrio = prioSelection
	local lbIsMajor = ministerCountry:IsMajor()
	
	-- If country just started mobilizing (or gets bonus reinforcements for some other reason), boost reinforcements
	if ( prioSelection == 0 or prioSelection == 3 )then
		--local reinforcement_multiplier = ministerCountry:CalculateReinforcementMultiplier():Get()
		--if reinforcement_multiplier > 1.0 then
		if bHasReinforceBonus then
			prioSelection = 4
		end
	end
	
	local vLendLeaseOriginal = vLendLease
	local vConsumerOriginal = vConsumer
	local vProductionOriginal = vProduction
	local vSupplyOriginal = vSupply
	local vReinforceOriginal = vReinforce
	local vUpgradeOriginal = vUpgrade
	local lbAtWar = ministerCountry:IsAtWar()
	
	-- If Dissent is present add 10% to the Production of Consumer Goods
	local dissent = ministerCountry:GetDissent():Get()
	if dissent > 0.01 then -- fight dissent 
		vConsumer = vConsumer + 0.1
	end
	
	-- Performance check make sure its above 0 before we even look at this
	if vSupply > 0 then
		local supplyStockpile = ministerCountry:GetPool():Get( CGoodsPool._SUPPLIES_ ):Get()
		--local weeksSupplyUse = ministerCountry:GetDailyExpense( CGoodsPool._SUPPLIES_ ):Get() * 7
	
		-- Major power check
		if lbIsMajor then
			-- Shut supply production down
			if supplyStockpile > 50000 then
				vSupply = 0

			-- To many go down 50 percent
			elseif supplyStockpile > 40000 then
				vSupply = vSupply * 0.5
				
			-- To many go down 60 percent
			elseif supplyStockpile > 37000 then
				vSupply = vSupply * 0.6
				
			-- To many go down 70 percent
			elseif supplyStockpile > 35000 then
				vSupply = vSupply * 0.7

			-- Decent slowly 99 percent
			elseif supplyStockpile > 32000 then
				vSupply = vSupply * 0.99

			-- Try to kee it steady 100 percent
			elseif supplyStockpile > 30000 then
				vSupply = vSupply

			-- Short on Supplies produce 115 percent
			elseif supplyStockpile > 28000 then
				vSupply = vSupply * 1.02
				
			-- Short on Supplies produce 115 percent
			elseif supplyStockpile > 22000 then
				vSupply = vSupply * 1.15

			-- Short on Supplies produce 125 percent
			else
				vSupply = vSupply * 1.25
				
			end
		else
			-- Stockpile to high cut it off
			if supplyStockpile > 5000 then
				vSupply = 0

			-- To many go down 40 percent
			elseif supplyStockpile > 4000 then
				vSupply = vSupply * 0.4
				
			-- To many go down 40 percent
			elseif supplyStockpile > 3700 then
				vSupply = vSupply * 0.5
				
			-- To many go down 60 percent
			elseif supplyStockpile > 3500 then
				vSupply = vSupply * 0.6

			-- Decent slowly 99 percent
			elseif supplyStockpile > 3200 then
				vSupply = vSupply * 0.99

			-- Try to kee it steady 100 percent
			elseif supplyStockpile > 3000 then
				vSupply = vSupply

			-- Short on Supplies produce 102 percent
			elseif supplyStockpile > 2800 then
				vSupply = vSupply * 1.02
				
			-- Short on Supplies produce 115 percent
			elseif supplyStockpile > 2200 then
				vSupply = vSupply * 1.15

			-- Short on Supplies produce 125 percent
			else
				vSupply = vSupply * 1.25
				
			end
		end
	end

	-- Lend-Lease priority
	if (liOrigPrio == 0) then -- if not using full auto we let player set this slider completely
		if ministerCountry:HasActiveLendLeaseToAnyone() then 
			local liMaxGivenLL = 0.3
			if lbAtWar then
				-- TODO should be callback instead
				if ministerCountry:GetCountryTag() == CCountryDataBase.GetTag("USA") then
					liMaxGivenLL = 0.3
					local vManpower = ministerCountry:GetManpower():Get()
					if vManpower < 50 then
						liMaxGivenLL = 0.5
					elseif vManpower < 100 then
						liMaxGivenLL = 0.4
					end
				else
					liMaxGivenLL = 0.1
				end
			end
			local liPreferredLL = ministerCountry:GetMaxLendLeaseFraction():Get() * liMaxGivenLL
			if vLendLease == 0 then
				vLendLease = liPreferredLL
			end
			if vLendLease < liPreferredLL * 0.95 then
				vLendLease = vLendLease * 1.05
			elseif vLendLease > liPreferredLL then
				vLendLease = vLendLease * 0.95
			end
		else
			vLendLease = 0
		end
	end
	
	-- observe this uses the original prio orders from PRIO_SETTING, so if you mod that you cant use this function
	-- and have to roll the commented out code above
	local vLendLease, vConsumer, vProduction, vSupply, vReinforce, vUpgrade, factor_left = CAI.FastNormalizeByPriority( prioSelection, vLendLease, vConsumer, vProduction, vSupply, vReinforce, vUpgrade )
	
	--factor_left = math.max(factor_left, 0.0)
	if liOrigPrio == 0 then

		local liProdUpgradeTotalPercentage = vUpgrade + vProduction + factor_left
		
		-- If the total needed for Upgrading exceedes the total amount available between
		--   Production and Upgrades then divide the number in half so something gets produced.
		if (vUpgradeOriginal > liProdUpgradeTotalPercentage or 
		    vUpgradeOriginal > (liProdUpgradeTotalPercentage / 2))
		then
			vUpgrade = (liProdUpgradeTotalPercentage / 2)
			vProduction = (liProdUpgradeTotalPercentage / 2)

		-- Upgrades is covered put everything extra into Production
		else
			vUpgrade = vUpgradeOriginal
			vProduction = liProdUpgradeTotalPercentage - vUpgradeOriginal
		end
	else
		-- We have some dessent so put extra IC to lower it
		if dissent > 0.01 then
			vConsumer = vConsumer + factor_left
		else
			vProduction = vProduction + factor_left
		end
	end

	local checksum = math.abs(vLendLease - vLendLeaseOriginal) +
	                 math.abs(vConsumer - vConsumerOriginal) + 
	                 math.abs(vProduction - vProductionOriginal) +
					 math.abs(vSupply - vSupplyOriginal) +
					 math.abs(vReinforce - vReinforceOriginal) +
					 math.abs(vUpgrade - vUpgradeOriginal)
	if checksum > 0.01 then
		local command = CChangeInvestmentCommand( ministerCountry:GetCountryTag(), vLendLease, vConsumer, vProduction, vSupply, vReinforce, vUpgrade )
		ai:Post( command )
	end
end


-- ###################################
-- # Main Method called by the EXE
-- #####################################
function BalanceLendLeaseSliders(ai, ministerCountry, countryTags, values)
	local countryFunRef = Utils.HasCountryAIFunction(ministerCountry:GetCountryTag(), "BalanceLendLeaseSliders")
	if countryFunRef then -- override
		countryFunRef(ai, ministerCountry, countryTags, values)
	else

		for i=0, countryTags:GetSize()-1 do
    		local ToTag = countryTags:GetAt(i)
    		values:SetAt( i, CFixedPoint( ToTag:GetCountry():GetTotalIC() ) ) -- it gets normalized anyway
  		end

		-- Do this to confirm LL sliders distribution
		local command = CChangeLendLeaseDistributionCommand( ministerCountry:GetCountryTag() )
		command:SetData( countryTags, values )
		ai:Post( command )
	end
end


-- ###################################
-- # Main Method called by the EXE
-- #####################################
function ProductionMinister_Tick(minister)
	-- Reset Global Array Container
	ProductionData = {
		minister = minister,
		ministerAI = nil,
		ministerTag = nil,
		ministerCountry = nil,
		humanTag = nil,
		Year = nil,
		CapitalPrvID = nil,
		IsAtWar = nil, -- Boolean are they atwar with someone
		IsMajor = nil, -- Boolean are they a major power
		IsNaval = nil, -- Boolean do they have the min requirements to build ships
		IsFirepower = nil, -- Boolean indicating if the have Superior Firepower tech
		IsExile	= nil, -- Boolean are the in exile
		TechStatus = nil, -- TechStatus Object
		BuildingCounts = {},
		CurrentCounts = {},
		TotalCounts = {},
		UnitNeeds = {},
		LandCountTotal = 0, -- Total amount of Land units including ones in build que
		AirCountTotal = 0, -- Total amount of Air units including ones in build que
		NavalCountTotal = 0, -- Total amount of Naval units including ones in build que
		SpecialForcesCountTotal = 0, -- Total amount of Special Forces units including ones in build que
		FlyingBombsCountTotal = 0, -- Total amount of Flying Bombs units including ones in build que
		ParaCountTotal = 0, -- Total amount of units, that can paradrop, including ones in build que
		icTotal = nil, -- Grant Total IC the country has
		icAllocated = nil, -- IC currently allocated to production
		icAvailable = nil, -- IC currently available for production
		PortsTotal = 0, -- Total amount of ports
		AirfieldsTotal = 0, -- Total amount of airfields
		ManpowerMobilize = nil, -- Boolean do they have enough MP stored to mobilize
		ManpowerTotal = 0, -- Total MP currently available to them
		ManpowerCap = 0, -- Coming Soon
		BuiltRocketSite = false,
		UnitTypes = UnitTypes}
	
	if math.random(2) == 1 then
	
		return	
	end
	-- Initialize Production Object
	-- #################
	ProductionData.ministerTag = minister:GetCountryTag()
	ProductionData.ministerCountry = ProductionData.ministerTag:GetCountry()
	ProductionData.icAllocated = ProductionData.ministerCountry:GetICPart(CDistributionSetting._PRODUCTION_PRODUCTION_):Get()
	ProductionData.icAvailable = ProductionData.icAllocated - ProductionData.ministerCountry:GetUsedIC():Get()
	-- End Initialize Production Object
	-- #################

	-- Performance check
	--   if no IC just exit completely so no objects get created
	if ProductionData.icAvailable < 0.1 then
		return
	end
	
	-- Initialize Production Object
	-- #################
	for x = 1, table.getn(UnitTypes) do
		ProductionData.BuildingCounts[x] = 0
		ProductionData.CurrentCounts[x] = 0
		ProductionData.TotalCounts[x] = 0
		ProductionData.UnitNeeds[x] = 0
	end

	ProductionData.ministerAI = minister:GetOwnerAI()
	ProductionData.humanTag = CCurrentGameState.GetPlayer()
	ProductionData.Year = CCurrentGameState.GetCurrentDate():GetYear()
	ProductionData.CapitalPrvID = ProductionData.ministerCountry:GetActingCapitalLocation():GetProvinceID()
	ProductionData.icTotal = ProductionData.ministerCountry:GetTotalIC()
	ProductionData.IsMajor = ProductionData.ministerCountry:IsMajor()
	ProductionData.IsExile = ProductionData.ministerCountry:IsGovernmentInExile()
	ProductionData.TechStatus = ProductionData.ministerCountry:GetTechnologyStatus()
	
	
	ProductionData.IsFirepower = (ProductionData.TechStatus:GetLevel(CTechnologyDataBase.GetTechnology("superior_firepower")) ~= 0)
	
	ProductionData.ManpowerMobilize = ProductionData.ministerCountry:HasExtraManpowerLeft()
	ProductionData.PortsTotal = ProductionData.ministerCountry:GetNumOfPorts()
	ProductionData.AirfieldsTotal = ProductionData.ministerCountry:GetNumOfAirfields()
	ProductionData.IsAtWar = ProductionData.ministerCountry:IsAtWar()
	ProductionData.IsNaval = (ProductionData.PortsTotal > 0 and ProductionData.icTotal >= 20)
	ProductionData.ManpowerTotal = ProductionData.ministerCountry:GetManpower():Get()
	-- End Initialize Production Object
	-- #################
	

	--random hoi Build naval bases if < 5
	if ProductionData.ministerCountry:GetTotalIC() > 15 then	
		if ProductionData.icAvailable > 4 then
			ProductionData.icAvailable = ConstructBuildings_ports(ProductionData.ministerAI , ProductionData.ministerTag, ProductionData.ministerCountry, ProductionData.icAvailable )
		end
	end
	
		-- Build Convoys first above all (they count against Other toward the end
	ProductionData.icAvailable = ConstructConvoys(ProductionData.icAvailable)
	
	-- Check to make sure they have Manpower
	--    IC check added for performance. If none dont bother executing.
	if ProductionData.ManpowerMobilize and ProductionData.icAvailable > 0.1 then
		--Utils.LUA_DEBUGOUT("Country: " .. tostring(ProductionData.ministerTag))
		
		-- Get the counts of the unit types currently being produced
		local laTempProd = ProductionData.ministerAI:GetProductionSubUnitCounts()
		local laTempCurrent = ProductionData.ministerAI:GetDeployedSubUnitCounts()
		--local laTempTReq = ProductionData.ministerAI:GetTheatreSubUnitNeedCounts()
		
		-- Get the build counts
		for subUnit in CSubUnitDataBase.GetSubUnitList() do
			local lsUnitType = subUnit:GetKey():GetString() 
			
			if not(UnitTypes[lsUnitType] == nil) then
				local nIndex = subUnit:GetIndex()
				local liBuildCount = laTempProd:GetAt(nIndex)
				local liCurrentCount = laTempCurrent:GetAt(nIndex)
				ProductionData.BuildingCounts[UnitTypes[lsUnitType].Index] = liBuildCount
				ProductionData.CurrentCounts[UnitTypes[lsUnitType].Index] =  liCurrentCount
				ProductionData.TotalCounts[UnitTypes[lsUnitType].Index] = liBuildCount + liCurrentCount
			end
		end	
		
		-- One loop to do all the counting (Performance)
		for k, v in pairs(UnitTypes) do
			if v.Type == "Land" then ProductionData.LandCountTotal = ProductionData.LandCountTotal + ProductionData.TotalCounts[v.Index] end
			if v.Type == "Air" then ProductionData.AirCountTotal = ProductionData.AirCountTotal + ProductionData.TotalCounts[v.Index] end
			if v.Type == "Naval" then ProductionData.NavalCountTotal = ProductionData.NavalCountTotal + ProductionData.TotalCounts[v.Index] end
			if v.Type == "Land" and v.SubType == "Special Forces" then ProductionData.SpecialForcesCountTotal = ProductionData.SpecialForcesCountTotal + ProductionData.TotalCounts[v.Index] end
			if v.Type == "Secret" then ProductionData.FlyingBombsCountTotal = ProductionData.FlyingBombsCountTotal + ProductionData.TotalCounts[v.Index] end
			if v.CanPara == true then ProductionData.ParaCountTotal = ProductionData.ParaCountTotal + ProductionData.TotalCounts[v.Index] end
		end
		-- End of Counting

		-- Verify Build Ratios against available units
		local laLandRatio = IsUnitsAvailable(GetBuildRatio("LandRatio"))
		local laAirRatio = GetBuildRatio("AirRatio")
		local laNavalRatio = GetBuildRatio("NavalRatio")
		local laUnits = {"ranger_brigade","guards_brigade","waffen_brigade","imperial_brigade","alpins_brigade","alpini_brigade","gurkha_brigade" };
		local laEliteUnits = GetEliteUnitBuildCount(laUnits)
		local laSpecialForcesRatio, laSpecialRatio = GetBuildRatio("SpecialForcesRatio")
		local laProdWeights = GetBuildRatio("ProductionWeights")
		local laRocketRatio = GetBuildRatio("RocketRatio")
		local laLandToAirRatio = GetBuildRatio("LandToAirRatio")
		local laTransportLandRatio = GetBuildRatio("TransportLandRatio")
		local laFirePower = GetBuildRatio("FirePower")
		
		-- If no air fields do not build any air units
		if ProductionData.AirfieldsTotal <= 0 and laProdWeights[2] > 0 then
			laAirRatio = {} -- Set it to an empty array
			
			-- Land ratio is greater than 0 so add it to land
			if laProdWeights[1] > 0 then
				-- Now move the Air IC over to the Land section
				laProdWeights[1] = laProdWeights[1] + laProdWeights[2]
				
			-- Add it to Naval
			elseif ProductionData.PortsTotal > 0 and laProdWeights[3] > 0 then
				laProdWeights[3] = laProdWeights[3] + laProdWeights[2]
			
			-- Add it to the Other Category
			else
				laProdWeights[4] = laProdWeights[4] + laProdWeights[2]
			end
			
			laProdWeights[2] = 0
		end
		
		-- If no ports do not build any naval units
		if ProductionData.PortsTotal <= 0 and laProdWeights[3] > 0 then
			laNavalRatio = {} -- Set it to an empty array
		
			-- If Land Ratio greater than 0 then add it there
			if laProdWeights[1] > 0 then
				laProdWeights[4] = laProdWeights[4] + laProdWeights[3]
				
			-- If Air Ratio greter than 0 add it to there
			elseif ProductionData.AirfieldsTotal > 0 and laProdWeights[2] > 0 then
				laProdWeights[2] = laProdWeights[2] + laProdWeights[3]
				
			-- Add it to the Other Category
			else
				-- Now move the Naval IC over to the Land section
				laProdWeights[1] = laProdWeights[1] + laProdWeights[3]
			end
			
			laProdWeights[3] = 0
		end	
		
		-- Figure out how much IC is suppose to be designated in the appropriate area
		local liPotentialLandIC = tonumber(tostring(ProductionData.icAllocated * laProdWeights[1]))
		local liPotentialAirIC = tonumber(tostring(ProductionData.icAllocated * laProdWeights[2]))
		local liPotentialNavalIC = tonumber(tostring(ProductionData.icAllocated * laProdWeights[3]))
		local liPotentialOtherIC = tonumber(tostring(ProductionData.icAllocated * laProdWeights[4]))
		
		local liNeededLandIC = 0
		local liNeededAirIC = 0
		local liNeededNavalIC = 0
		local liNeededOtherIC = 0
			
		-- Figure out what the AI is currently producing in each category
		for loBuildItem in ProductionData.ministerCountry:GetConstructions() do
			if loBuildItem:IsMilitary() then
				local loMilitary = loBuildItem:GetMilitary()
				
				if loMilitary:IsLand() then
					liNeededLandIC = liNeededLandIC + loBuildItem:GetCost()
				elseif loMilitary:IsAir() then
					for loConstDef in loMilitary:GetBrigades() do
						local loSubUnit = loConstDef:GetType()
						
						-- If it is a cag add it to naval IC count instead of air
						if loSubUnit:IsCag() then
							liNeededNavalIC = liNeededNavalIC + loBuildItem:GetCost()
						else
							liNeededAirIC = liNeededAirIC + loBuildItem:GetCost()
						end
						
						-- Exit the loop right away
						break
					end
				elseif loMilitary:IsNaval() then
					liNeededNavalIC = liNeededNavalIC + loBuildItem:GetCost()
				end
			else
				liNeededOtherIC = liNeededOtherIC + loBuildItem:GetCost()
			end
		end
		
		-- Now figure out what it needs
		liNeededLandIC = liPotentialLandIC - liNeededLandIC
		liNeededAirIC = liPotentialAirIC - liNeededAirIC
		liNeededNavalIC = liPotentialNavalIC - liNeededNavalIC
		liNeededOtherIC = liPotentialOtherIC - liNeededOtherIC
		
		-- Normalize the IC counts in case of shifts
		local liOverIC = 0
		
		-- Variables are negative numbers so add them
		if liNeededLandIC < 0 then
			liOverIC = liOverIC + liNeededLandIC
		end
		if liNeededAirIC < 0 then
			liOverIC = liOverIC + liNeededAirIC
		end
		if liNeededNavalIC < 0 then
			liOverIC = liOverIC + liNeededNavalIC
		end
		if liNeededOtherIC < 0 then
			liOverIC = liOverIC + liNeededOtherIC
		end

		if liOverIC > 0 then
			if liNeededNavalIC > 0 and liOverIC > 0 then
				if liNeededNavalIC >= liOverIC then
					liNeededNavalIC = liNeededNavalIC - liOverIC
					liOverIC = 0
				else
					liOverIC = liOverIC - liNeededNavalIC
					liNeededNavalIC = 0
				end
			end
			if liNeededAirIC > 0 and liOverIC > 0 then
				if liNeededAirIC >= liOverIC then
					liNeededAirIC = liNeededAirIC - liOverIC
					liOverIC = 0
				else
					liOverIC = liOverIC - liNeededAirIC
					liNeededAirIC = 0
				end
			end
			if liNeededLandIC > 0 and liOverIC > 0 then
				if liNeededLandIC >= liOverIC then
					liNeededLandIC = liNeededLandIC - liOverIC
					liOverIC = 0
				else
					liOverIC = liOverIC - liNeededLandIC
					liNeededLandIC = 0
				end
			end
			if liNeededOtherIC > 0 and liOverIC > 0 then
				if liNeededOtherIC >= liOverIC then
					liNeededOtherIC = liNeededOtherIC - liOverIC
					liOverIC = 0
				else
					liOverIC = liOverIC - liNeededOtherIC
					liNeededOtherIC = 0
				end
			end			
		end
		-- End of IC Normalization
		
		-- Process Land Units
		-- Used to figure out Air to Land Ratio
		local liTotalLandRatio = CalculateRatio(ProductionData.LandCountTotal, laLandToAirRatio[1])
		local liTotalAirRatio = CalculateRatio(ProductionData.AirCountTotal, laLandToAirRatio[2])
		
		-- If the Air ratio is higher than the Land ration then move all the Air IC into Land
		---   This means the country could have suffered massive losses via an encirclement
		if liTotalAirRatio > liTotalLandRatio then
			liNeededLandIC = liNeededLandIC + liNeededAirIC
			liNeededAirIC = 0
		end
		
		-- PERFORMANCE: only process if IC has been allocated
		local laLandUnitRatio = {} -- Regular Land Units
		local laSpecialUnitRatio = {} -- Special Forces
		
		-- Naval check is adding for Convoy ratio calculating.
		if liNeededLandIC > 0 or liNeededNavalIC > 0 then
			-- PERFORMANCE: Make sure you need the rest of this to run
			if liNeededLandIC > 0 then
				-- Calculate what the ratio is for each unit type
				for k, v in pairs(laLandRatio) do
					laLandUnitRatio[k] = CalculateRatio(ProductionData.TotalCounts[UnitTypes[k].Index], laLandRatio[k])
				end
				
				-- Multiplier used to figure out how many units of each type you need
				--   to keep the ratio
				local liMultiplier = GetMultiplier(laLandUnitRatio, laLandRatio)

				-- Now Figure out what the Unit needs are
				for k, v in pairs(laLandUnitRatio) do
					ProductionData.UnitNeeds[UnitTypes[k].Index] = (laLandRatio[k] * liMultiplier) - ProductionData.TotalCounts[UnitTypes[k].Index]
				end
				
				-- Setup Elite Units and add them to the Regular Land Array but with a priority of 0
				for k, v in pairs(laEliteUnits) do
					ProductionData.UnitNeeds[UnitTypes[k].Index] = laEliteUnits[k]
					laLandUnitRatio[k] = -1000
				end
				
				-- Special Forces
				-- Calculate how many Special Forces are needed
				local liTotalSFNeeded = 0

				if laSpecialRatio ~= nil and laSpecialForcesRatio[2] > 0 then
					liTotalSFNeeded = math.max(0, math.ceil((ProductionData.LandCountTotal / laSpecialForcesRatio[1]) * laSpecialForcesRatio[2]) - ProductionData.SpecialForcesCountTotal)

					-- Do we need special forces
					if liTotalSFNeeded > 0 then
						laSpecialRatio = IsUnitsAvailable(laSpecialRatio)
						
						for k, v in pairs(laSpecialRatio) do
							laSpecialUnitRatio[k] = CalculateRatio(ProductionData.TotalCounts[UnitTypes[k].Index], laSpecialRatio[k])
						end
						
						liMultiplier = GetMultiplier(laSpecialUnitRatio, laSpecialRatio)
						
						-- Now Figure out what the Unit needs are
						for k, v in pairs(laSpecialUnitRatio) do
							ProductionData.UnitNeeds[UnitTypes[k].Index] = (laSpecialRatio[k] * liMultiplier) - ProductionData.TotalCounts[UnitTypes[k].Index]
						end

						-- Modify the counts based on the max amount allowed
						ModifyUnitNeeds(laSpecialUnitRatio, liTotalSFNeeded)
					end	
				end
			end
		end
		
		-- Process Air Units
		local laAirUnitRatio = {}
		
		-- PERFORMANCE: only process if IC has been allocated
		if liNeededAirIC > 0 then
			-- Calculate what the ratio is for each unit type
			for k, v in pairs(laAirRatio) do
				laAirUnitRatio[k] = CalculateRatio(ProductionData.TotalCounts[UnitTypes[k].Index], laAirRatio[k])
			end
		
			local liMultiplier = GetMultiplier(laAirUnitRatio, laAirRatio)

			-- Now Figure out what the Unit needs are
			for k, v in pairs(laAirRatio) do
				ProductionData.UnitNeeds[UnitTypes[k].Index] = (laAirRatio[k] * liMultiplier) - ProductionData.TotalCounts[UnitTypes[k].Index]
			end
			
			-- Do we need Air Transports
			if ProductionData.ParaCountTotal > 0 then
				local liTotalAirTrans = ProductionData.TotalCounts[UnitTypes.transport_plane.Index]
				local liTotalAirTransNeeded = math.floor(ProductionData.ParaCountTotal / 3)
				
				if liTotalAirTransNeeded > liTotalAirTrans then
					ProductionData.UnitNeeds[UnitTypes.transport_plane.Index] = liTotalAirTransNeeded - liTotalAirTrans
					laAirUnitRatio["transport_plane"] = -1000
				end
			end
			
			-- Does the country have a Secret Ratio
			if laRocketRatio[1] > 0 then
				local liSNeeded = math.max(0, math.ceil((ProductionData.AirCountTotal / laRocketRatio[1]) * laRocketRatio[2]) - ProductionData.FlyingBombsCountTotal)
				
				-- Do they need any
				if liSNeeded > 0 then
					local lsUnitType = GetHighestUnit("Secret")
					
					if lsUnitType ~= nil then
						-- Pick a secret weapon randomly
						ProductionData.UnitNeeds[UnitTypes[lsUnitType].Index] = liSNeeded
						laAirUnitRatio[lsUnitType] = 1
					end
				end
			end
		end
			
		-- Process Naval Units
		local laNavalUnitRatio = {} -- This Array is passed to the BuildNavalUnit method
		
		--    PERFORMANCE: only process if IC has been allocated
		if liNeededNavalIC > 0 then
			-- Calculate what the ratio is for each unit type
			for k, v in pairs(laNavalRatio) do
				laNavalUnitRatio[k] = CalculateRatio(ProductionData.TotalCounts[UnitTypes[k].Index], laNavalRatio[k])
			end
			
			local liMultiplier = GetMultiplier(laNavalUnitRatio, laNavalRatio)

			-- Now Figure out what the Unit needs are
			for k, v in pairs(laNavalRatio) do
				ProductionData.UnitNeeds[UnitTypes[k].Index] = (laNavalRatio[k] * liMultiplier) - ProductionData.TotalCounts[UnitTypes[k].Index]
			end
			
			-- Transport Shipts
			if laTransportLandRatio[2] > 0 then
				local liTotalTransportsNeeded = math.ceil((ProductionData.LandCountTotal / laTransportLandRatio[1]) * laTransportLandRatio[2]) - ProductionData.TotalCounts[UnitTypes.transport_ship.Index]
				
				if liTotalTransportsNeeded > 0 then
					local lsUnitType = GetHighestUnit("Transport")

					if lsUnitType ~= nil then
						ProductionData.UnitNeeds[UnitTypes[lsUnitType].Index] = liTotalTransportsNeeded
						laNavalUnitRatio[lsUnitType] = 0
					end
				end
			end
			
			-- Invasion Craft
			if laTransportLandRatio[3] > 0 then
				local liTotalInvasionTransportsNeeded = math.ceil((ProductionData.LandCountTotal / laTransportLandRatio[1]) * laTransportLandRatio[3]) - ProductionData.TotalCounts[UnitTypes.invasion_transport_ship.Index]
				
				if liTotalInvasionTransportsNeeded > 0 then
					local lsUnitType = GetHighestUnit("Invasion")

					if lsUnitType ~= nil then
						ProductionData.UnitNeeds[UnitTypes[lsUnitType].Index] = liTotalInvasionTransportsNeeded
						laNavalUnitRatio[lsUnitType] = 0
					end
				end
			end

			-- Figure out if we need any CAGs
			local liCAGsNeeded = (ProductionData.TotalCounts[UnitTypes.carrier.Index] * 2) + ProductionData.TotalCounts[UnitTypes.escort_carrier.Index]
			local liCAGsCount = ProductionData.TotalCounts[UnitTypes.cag.Index]
			
			if liCAGsNeeded > liCAGsCount then
				ProductionData.UnitNeeds[UnitTypes.cag.Index] = liCAGsNeeded - liCAGsCount
				-- Give it a ratio of 1 so the AI will push them to be built first
				laNavalUnitRatio["cag"] = 0
			end
		end
		
		-- Build Naval Units
		if liNeededNavalIC > 0.1 then
			local liNewICCount = ProcessUnits(liNeededNavalIC, laNavalUnitRatio)
			ProductionData.icAvailable = ProductionData.icAvailable - (liNeededNavalIC - liNewICCount)
			liNeededNavalIC = liNewICCount
		end

		-- Build Air Units
		if liNeededAirIC > 0.1 then
			local liNewICCount = ProcessUnits(liNeededAirIC, laAirUnitRatio)
			ProductionData.icAvailable = ProductionData.icAvailable - (liNeededAirIC - liNewICCount)
			liNeededAirIC = liNewICCount
		end

		-- Build Land Units
		if liNeededLandIC > 0.1 then
			local liNewICCount = ProcessUnits(liNeededLandIC, laSpecialUnitRatio, laFirePower)
			liNewICCount = ProcessUnits(liNewICCount, laLandUnitRatio, laFirePower)
			ProductionData.icAvailable = ProductionData.icAvailable - (liNeededLandIC - liNewICCount)
			liNeededLandIC = liNewICCount
		end
		
		-- Build Buildings
		if liNeededOtherIC > 0.1 then
			local liNewICCount = BuildOtherUnits(liNeededOtherIC, true)
			ProductionData.icAvailable = ProductionData.icAvailable - (liNeededOtherIC - liNewICCount)
			liNeededOtherIC = liNewICCount
		end
		
		-- Last chance to use the IC
		if ProductionData.icAvailable > 0.1 then
			ProductionData.icAvailable = BuildOtherUnits(ProductionData.icAvailable, laFirePower)
			ProductionData.icAvailable = ProcessUnits(ProductionData.icAvailable, laNavalUnitRatio, laFirePower)
			ProductionData.icAvailable = ProcessUnits(ProductionData.icAvailable, laAirUnitRatio, laFirePower)
			ProductionData.icAvailable = ProcessUnits(ProductionData.icAvailable, laSpecialUnitRatio, laFirePower)
			ProductionData.icAvailable = ProcessUnits(ProductionData.icAvailable, laLandUnitRatio, laFirePower)
		end
		
	elseif ProductionData.icAvailable > 0.1 then
		-- AI did not have enough manpower to build any units so just do buildings
		ProductionData.icAvailable = BuildOtherUnits(ProductionData.icAvailable, true)
	end
	
	if math.mod( CCurrentGameState.GetAIRand(), 7) == 0 then
		ProductionData.minister:PrioritizeBuildQueue()
	end
end

-- #######################
-- Helper Build Methods
-- #######################
function ProcessUnits(ic, vaUnitRatio, vaFirePower)
	--local liLoopCount = table.getn(vaUnitRatio)

	-- Performance check, make sure there is enough IC to actually do something
	if ic > 0.1 then
		local lsLowestUnit
		local liLowestValue = -1
		local laUnitProcess = {}

		-- Main Loop Determines how many passess we actually have to make
		for i, z in pairs(vaUnitRatio) do
			for k, v in pairs(vaUnitRatio) do
				if not(laUnitProcess[k] == true) then
					if (not(lsLowestUnit == k) and liLowestValue >= vaUnitRatio[k])
					or liLowestValue == -1 then
						liLowestValue = vaUnitRatio[k]
						lsLowestUnit = k
					end
				end
			end

			laUnitProcess[lsLowestUnit] = true

			ic = BuildUnit(ic, 
					lsLowestUnit, 
					vaFirePower)			

			liLowestValue = -1
		end
	end
	
	return ic
end

function ModifyUnitNeeds(vaUnitRatio, viUnitNeeds)
	local lsLowestUnit
	local liLowestValue = -1
	local laUnitProcess = {}

	-- Main Loop Determines how many passess we actually have to make
	for i, z in pairs(vaUnitRatio) do
		for k, v in pairs(vaUnitRatio) do
			if not(laUnitProcess[k] == true) then
				if (not(lsLowestUnit == k) and liLowestValue >= vaUnitRatio[k])
				or liLowestValue == -1 then
					liLowestValue = vaUnitRatio[k]
					lsLowestUnit = k
				end
			end
		end
		
		laUnitProcess[lsLowestUnit] = true

		-- Subtract from the special forces till it = 0
		if ProductionData.UnitNeeds[UnitTypes[lsLowestUnit].Index] > viUnitNeeds then
			ProductionData.UnitNeeds[UnitTypes[lsLowestUnit].Index] = viUnitNeeds
			viUnitNeeds = 0;
		else
			viUnitNeeds = viUnitNeeds - ProductionData.UnitNeeds[UnitTypes[lsLowestUnit].Index];
		end
		
		liLowestValue = -1
	end
end

-- Figures out if the unit call has to go to an AI specific file or not
function BuildUnit(vIC, vsType, vaFirePower)
	-- Copy the object so the original is not changed
	local loType = {Name = vsType}
	for k, v in pairs(UnitTypes[vsType]) do
		loType[k] = UnitTypes[vsType][k]
	end

	-- Setup Parameter defaults
	if loType.Serial == nil then loType.Serial = 1 end
	if loType.Size == nil then loType.Size = 1 end
	if loType.Support == nil then loType.Support = 0 end
	
	local lbLicenseRequired = false
	lbLicenseRequired, ProductionData.ManpowerTotal =  Support_License.ProductionCheck(loType, ProductionData)
	
	if not(lbLicenseRequired) then
		if vIC > 1 and ProductionData.UnitNeeds[loType.Index] > 0 then 
			-- Firepower Check, if present and on list add one to support count
			if ProductionData.IsFirepower and vaFirePower ~= nil and loType.Support > 0 then
				for i = 0, table.getn(vaFirePower) do
					if vaFirePower[i] == vsType then
						loType.Support = loType.Support + 1 
					end
				end
			end
		
			local lsMethodOveride = "Build_" .. vsType
			--Utils.LUA_DEBUGOUT(	tostring(lsMethodOveride))	
			-- Check to see if the Country AI file has an overide or Defaults Do
			local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, lsMethodOveride)
			if loFunRef then
				vIC, ProductionData.ManpowerTotal, ProductionData.UnitNeeds[loType.Index] = loFunRef(vIC, ProductionData.ManpowerTotal, loType, ProductionData, ProductionData.UnitNeeds[loType.Index])
			else
				vIC, ProductionData.ManpowerTotal, ProductionData.UnitNeeds[loType.Index] = Support.CreateUnit(loType, vIC, ProductionData.UnitNeeds[loType.Index], ProductionData, nil)
			end
		end
	end

	return vIC
end

-- For the specified SubType it gets the highest unit in the array that is Available
function GetHighestUnit(vsSubType)
	local lsUnitAvailable = nil
	
	for k, v in pairs(UnitTypes) do
		if v.SubType == vsSubType then
			if ProductionData.TechStatus:IsUnitAvailable(CSubUnitDataBase.GetSubUnit(k)) then
				lsUnitAvailable = k
			end
		end
	end
	
	return lsUnitAvailable
end

-- Converts the Unit ratio to a % based number. If there
--   are 0 units but a Ratio exists then it will set it to 1.
function CalculateRatio(viUnitCount, viUnitRatio)
	local rValue
	
	if viUnitRatio == 0 then
		rValue = 0
	elseif viUnitCount == 0 then
		rValue = 1
	else
		rValue = viUnitCount / viUnitRatio
	end
	
	return rValue
end
-- Returns the Ratio Array requested
function GetBuildRatio(vsType)
	return Utils.CallBuildFunction(ProductionData.ministerTag, ProductionData.IsNaval, vsType, ProductionData)
end
		
function GetMultiplier(vaUnitRatio, vaRatio)
	local liMultiplier = 0
	local liAddToMultiplier = 2
	
	for k, v in pairs(vaUnitRatio) do	
		if vaRatio[k] > 0 then
			liMultiplier = math.max(liMultiplier, vaUnitRatio[k])
		end
	end
	
	-- Make sure some sort of multiplier gets past, AddToMultipler if 0 means Multiplier is something
	return math.max((liMultiplier + liAddToMultiplier), liAddToMultiplier)
end

-- Goes through the ratios and sets them to 0 if the country can't build any of those units
function IsUnitsAvailable(vaRatio)
	for k, v in pairs(vaRatio) do
		if not(ProductionData.TechStatus:IsUnitAvailable(CSubUnitDataBase.GetSubUnit(k))) then
			vaRatio[k] = 0
		end
	end
	
	return vaRatio
end

-- Looks for the elite units specified to see if they can be built and if so sets the need amount
function GetEliteUnitBuildCount(vaElitUnits)
	local laNewEliteUnits = {}

	if vaElitUnits ~= nil then
		for i = 1, table.getn(vaElitUnits) do
			laNewEliteUnits[vaElitUnits[i]] = ProductionData.ministerCountry:CountMaxUnitsStillBuildable(CSubUnitDataBase.GetSubUnit(vaElitUnits[i]))
		end
	end
	
	return laNewEliteUnits
end


-- #######################
-- End of Helper Build Methods
-- #######################

-- #######################
-- Building Construction
-- #######################
function BuildOtherUnits(ic)
	-- Buildings
	if ic > 0.1 then
		local liTotalBuildings = 11
	
		--Setup the building object
		local loBuildings = {
			coastal_fort = CBuildingDataBase.GetBuilding("coastal_fort" ),
			land_fort = CBuildingDataBase.GetBuilding( "land_fort" ),
			anti_air = CBuildingDataBase.GetBuilding("anti_air" ),
			industry = CBuildingDataBase.GetBuilding("industry" ),
			radar_station = CBuildingDataBase.GetBuilding("radar_station" ),
			nuclear_reactor = CBuildingDataBase.GetBuilding("nuclear_reactor" ),
			rocket_test = CBuildingDataBase.GetBuilding("rocket_test" ),
			infra = CBuildingDataBase.GetBuilding("infra"),
			air_base = CBuildingDataBase.GetBuilding("air_base"),
			naval_base = CBuildingDataBase.GetBuilding("naval_base"),
			underground = CBuildingDataBase.GetBuilding("underground"),
		}
		
		-- Setup which buildings can be built
		loBuildings.lbCoastal_fort = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.coastal_fort)
		loBuildings.lbLand_fort = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.land_fort)
		loBuildings.lbAnti_air = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.anti_air)
		loBuildings.lbIndustry = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.industry)
		loBuildings.lbRadar_station = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.radar_station)
		loBuildings.lbNuclear_reactor = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.nuclear_reactor)
		loBuildings.lbRocket_test = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.rocket_test)
		loBuildings.lbInfra = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.infra)
		loBuildings.lbAir_base = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.air_base)
		loBuildings.lbNaval_base = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.naval_base)
		loBuildings.lbUnderground = ProductionData.TechStatus:IsBuildingAvailable(loBuildings.underground)
		
		
		local liRandomBuildArray = {}
		
		for i = 1, liTotalBuildings do
			liRandomBuildArray[i] = i
		end
		
		-- Produce buildings until your out of IC that has been allocated
		--   Never have more than 1 rocket sites
		local liRocketCap = 1
		local liReactorCap = 2
		local loCorePrv = CoreProvincesLoop(loBuildings, liRocketCap, liReactorCap)
		local lbProcess = true -- Flag used to indicate to process regular code as well

		for i = 1, liTotalBuildings do
			local liBuilding = math.random(table.getn(liRandomBuildArray))

			if liRandomBuildArray[liBuilding] == 1 then
				-- Underground base
				if ic > 0.1 and loBuildings.lbUnderground then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_Underground")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							local liProvinceID = ProductionData.ministerCountry:GetRandomUnderGroundTarget()
							if liProvinceID > 0 then
								local loCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.underground, liProvinceID, 1)

								if loCommand:IsValid() then
									ProductionData.ministerAI:Post(loCommand)
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.underground):Get()
									ic = ic - liCost -- Update IC total	
									break
								end
							end
						end				
					else
						lbProcess = true -- Reset Flag for next check
					end
				end
			elseif liRandomBuildArray[liBuilding] == 2 then
				-- Nuclear Reactors stations
				if ic > 0.1 and loBuildings.lbNuclear_reactor then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_NuclearReactor")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
					
					if lbProcess then
						if loCorePrv.ReactorSites < liReactorCap then
							if ic > 0.1 then
								if table.getn(loCorePrv.PrvForBuilding) > 0 then
									local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.nuclear_reactor, loCorePrv.PrvForBuilding[math.random(table.getn(loCorePrv.PrvForBuilding))], 1 )

									if constructCommand:IsValid() then
										ProductionData.ministerAI:Post( constructCommand )
										
										local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.nuclear_reactor):Get()
										ic = ic - liCost -- Upodate IC total	
									end
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end
				end
			elseif liRandomBuildArray[liBuilding] == 3 then
				-- Rocket Test Site stations
				if ic > 0.1 and loBuildings.lbRocket_test then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_RocketTest")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
					
					if lbProcess then
						if not(ProductionData.BuiltRocketSite) then
							if loCorePrv.RocketSites < liRocketCap then
								-- Limits minors to only consider building Rocket Test sites after 1943
								if ProductionData.IsMajor or ProductionData.Year > 1943 then
									if ic > 0.1 then
										if table.getn(loCorePrv.PrvForBuilding) > 0 then
											local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.rocket_test, loCorePrv.PrvForBuilding[math.random(table.getn(loCorePrv.PrvForBuilding))], 1 )

											if constructCommand:IsValid() then
												ProductionData.ministerAI:Post( constructCommand )
												
												local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.rocket_test):Get()
												ic = ic - liCost -- Update IC total	
												ProductionData.BuiltRocketSite = true
											end
										end
									end
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end
				end
			elseif liRandomBuildArray[liBuilding] == 4 then
				-- Industry
				if ic > 0.1 and loBuildings.lbIndustry then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_Industry")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
					
					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvForBuildingIndustry) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.industry, loCorePrv.PrvForBuildingIndustry[math.random(table.getn(loCorePrv.PrvForBuildingIndustry))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.industry):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end
				end						
			elseif liRandomBuildArray[liBuilding] == 5 then
				-- Build Forts
				--   Since there is no practical way to teach the AI to build forts just allow hooks for country specific stuff
				if ic > 0.1 and loBuildings.lbLand_fort then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_Fort")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					-- Don't build a fort on the capital unless there is nothing else to do
					if lbProcess then
						if ic > 0.1 then
							-- Get Costal Fort information
							local loProvince = ProductionData.ministerCountry:GetActingCapitalLocation()
							local loLandFort = loProvince:GetBuilding(loBuildings.land_fort)

							-- Make sure the Capital does not already have a size 2 fort
							if loLandFort:GetMax():Get() < 2 and loProvince:GetCurrentConstructionLevel(loBuildings.land_fort) == 0 then
								if ProductionData.ministerCountry:IsBuildingAllowed(loBuildings.land_fort, loProvince) then
									local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.land_fort, loProvince:GetProvinceID(), 1 )

									if constructCommand:IsValid() then
										ProductionData.ministerAI:Post( constructCommand )
										
										local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.land_fort):Get()
										ic = ic - liCost -- Upodate IC total
										break 
									end
								end
							end
						end
					else
						lbProcess = true
					end
				end
			elseif liRandomBuildArray[liBuilding] == 6 then
				-- Build Coastal Forts
				if ic > 0.1 and loBuildings.lbCoastal_fort then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_CoastalFort")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvCoastalFort) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.coastal_fort, loCorePrv.PrvCoastalFort[math.random(table.getn(loCorePrv.PrvCoastalFort))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.coastal_fort):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end				
				end
			elseif liRandomBuildArray[liBuilding] == 7 then
				-- Build Anti Air
				if ic > 0.1 and loBuildings.lbAnti_air then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_AntiAir")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvAntiAir) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.anti_air, loCorePrv.PrvAntiAir[math.random(table.getn(loCorePrv.PrvAntiAir))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.anti_air):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end				
				end
			elseif liRandomBuildArray[liBuilding] == 8 then
				-- Radar stations
				if ic > 0.1 and loBuildings.lbRadar_station then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_Radar")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvRadarStation) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.radar_station, loCorePrv.PrvRadarStation[math.random(table.getn(loCorePrv.PrvRadarStation))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.radar_station):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end	
				end
			elseif liRandomBuildArray[liBuilding] == 9 then
				-- Build Airfields
				if ic > 0.1 and loBuildings.lbAir_base then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_AirBase")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvAirBase) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.air_base, loCorePrv.PrvAirBase[math.random(table.getn(loCorePrv.PrvAirBase))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.air_base):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end					
				end
			elseif liRandomBuildArray[liBuilding] == 10 then
				-- Infrastructure
				if ic > 0.1 and loBuildings.lbInfra then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_Infrastructure")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end
				
					if lbProcess then
						if ic > 0.1 then
							local liRandomIndex
							local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.infra):Get()
							
							-- Limit it to three provinces at a time
							for i = 1, 3 do
								if table.getn(loCorePrv.PrvLowInfra69) > 0 then
									liRandomIndex = math.random(table.getn(loCorePrv.PrvLowInfra69))
									local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.infra, loCorePrv.PrvLowInfra69[liRandomIndex], 1 )

									if constructCommand:IsValid() then
										if ic > 0.1 then
											ProductionData.ministerAI:Post( constructCommand )
											ic = ic - liCost -- Upodate IC total	
											table.remove(loCorePrv.PrvLowInfra69, liRandomIndex)
										else
											break
										end
									end
								elseif table.getn(loCorePrv.PrvLowInfra99) > 0 then
									liRandomIndex = math.random(table.getn(loCorePrv.PrvLowInfra99))
									local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.infra, loCorePrv.PrvLowInfra99[liRandomIndex], 1 )

									if constructCommand:IsValid() then
										if ic > 0.1 then
											ProductionData.ministerAI:Post( constructCommand )
											ic = ic - liCost -- Upodate IC total	
											table.remove(loCorePrv.PrvLowInfra99, liRandomIndex)
										else
											break
										end
									end
								end
								
								-- If there is no IC left do not loop another time
								if ic <= 0.2 then
									break
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end
				end	
			elseif liRandomBuildArray[liBuilding] == 11 then
				-- Expand a port since there is nothing else left to do
				if ic > 0.1 and loBuildings.lbNaval_base then
					local loFunRef = Utils.GetFunctionReference(ProductionData.ministerTag, ProductionData.IsNaval, "Build_NavalBase")
					if loFunRef then
						ic, lbProcess = loFunRef(ic, ProductionData)
					end

					if lbProcess then
						if ic > 0.1 then
							if table.getn(loCorePrv.PrvNavalBase) > 0 then
								local constructCommand = CConstructBuildingCommand(ProductionData.ministerTag, loBuildings.naval_base, loCorePrv.PrvNavalBase[math.random(table.getn(loCorePrv.PrvNavalBase))], 1 )

								if constructCommand:IsValid() then
									ProductionData.ministerAI:Post( constructCommand )
									
									local liCost = ProductionData.ministerCountry:GetBuildCost(loBuildings.naval_base):Get()
									ic = ic - liCost -- Upodate IC total	
								end
							end
						end
					else
						lbProcess = true -- Reset Flag for next check
					end						
				end
			end
			
			table.remove(liRandomBuildArray, i)
		end
	end
	
	return ic
end

function CoreProvincesLoop(voBuildings, viRocketCap, viReactorCap)
	local lbBuildIndustry = false  -- Should the AI consider building Industry
	local laAtWarWith = {} -- Holds an array of country tags they are atwar with and are holding a core province
	
	local loCorePrv = {
		RocketSites = ProductionData.ministerCountry:GetTotalCoreBuildingLevels(voBuildings.rocket_test:GetIndex()):Get(),
		ReactorSites = ProductionData.ministerCountry:GetTotalCoreBuildingLevels(voBuildings.nuclear_reactor:GetIndex()):Get(),
		PrvLowInfra69 = {}, -- Provinces less than 70 infrastructure
		PrvLowInfra99 = {}, -- Provinces less than 100 infrastructure
		PrvForBuilding = {}, -- Provinces that qualify for any building
		PrvForBuildingIndustry = {}, -- Provinces that qualify for Industry to be built in them
		PrvAntiAir = {}, -- Provinces that qualify for Anti Air
		PrvCoastalFort = {}, -- Provinces that qualify for Coastal Fort
		PrvRadarStation = {}, -- Provinces that qualify for Radar Station
		PrvAirBase = {}, -- Provinces that qualify for an Air Base
		PrvNavalBase = {} -- Provinces that qualify for a Naval Base
	}	
	
	-- Performance check, no need to count resources if you can't build a factory
	if voBuildings.lbIndustry then
		local liExpenseFactor, liHomeFactor = Support.CalculateExpenseResourceFactor(ProductionData.ministerCountry)
		
		-- We produce more than what we use so build more industry
		if liHomeFactor > liExpenseFactor then
			lbBuildIndustry = true
		end
	end
	
	for liProvinceId in ProductionData.ministerCountry:GetControlledProvinces() do
		local loProvince = CCurrentGameState.GetProvince(liProvinceId)
		local loProvinceInfra = loProvince:GetBuilding(voBuildings.infra)
		local liInfraSize = loProvinceInfra:GetMax():Get()
		
		if liInfraSize > 1 then
			local isFrontProvince = loProvince:IsFrontProvince(false)
			local liConstructionLevel = loProvince:GetCurrentConstructionLevel(voBuildings.infra)
			local loOwnerTag = loProvince:GetOwner()
			
			-- Any province can have their infra improved not just owned ones
			if voBuildings.lbInfra then
				if liInfraSize < 7 and not(liConstructionLevel > 0) and not(isFrontProvince) then
					table.insert(loCorePrv.PrvLowInfra69, liProvinceId)
				elseif liInfraSize < 10 and not(liConstructionLevel > 0) and not(isFrontProvince) then
					table.insert(loCorePrv.PrvLowInfra99, liProvinceId)
				end
			end
			
			if not(isFrontProvince) then
				local lbHasNavalBase = loProvince:HasBuilding(voBuildings.naval_base)
				local lbHasIndustry = loProvince:HasBuilding(voBuildings.industry)
				local lbHasAirField = loProvince:HasBuilding(voBuildings.air_base)
				local lbHasNavalBase = loProvince:HasBuilding(voBuildings.naval_base)

				-- Plan Anti Aircraft layout
				if lbHasNavalBase or lbHasIndustry or lbHasAirField then
					-- Provinces that qualify for Anti-Air
					if voBuildings.lbAnti_air then
						if loProvince:GetBuilding(voBuildings.anti_air):GetMax():Get() < 5 then
							if loProvince:GetCurrentConstructionLevel(voBuildings.anti_air) == 0 then
								table.insert(loCorePrv.PrvAntiAir, liProvinceId)
							end
						end
					end
					
					if lbHasNavalBase then
						-- Provinces that qualify for Coastal Base
						if voBuildings.lbCoastal_fort then
							if loProvince:GetBuilding(voBuildings.coastal_fort):GetMax():Get() < 2 then
								if loProvince:GetCurrentConstructionLevel(voBuildings.coastal_fort) == 0 then
									table.insert(loCorePrv.PrvCoastalFort, liProvinceId)
								end
							end
						end
						
						-- Provinces that qualify for a Naval Base
						if voBuildings.lbNaval_base then
							if loProvince:GetBuilding(voBuildings.naval_base):GetMax():Get() < 10 then
								if loProvince:GetCurrentConstructionLevel(voBuildings.naval_base) == 0 then
									table.insert(loCorePrv.PrvNavalBase, liProvinceId)
								end
							end
						end
					end
					
					if lbHasAirField then
						-- Provinces that qualify for an Air Base
						if voBuildings.lbAir_base then
							if loProvince:GetBuilding(voBuildings.air_base):GetMax():Get() < 10 then
								if loProvince:GetCurrentConstructionLevel(voBuildings.air_base) == 0 then
									table.insert(loCorePrv.PrvAirBase, liProvinceId)
								end
							end
						end
						
						--Provinces that qualify for Radar Station
						if voBuildings.lbRadar_station then
							if loProvince:GetBuilding(voBuildings.radar_station):GetMax():Get() < 5 then
								if loProvince:GetCurrentConstructionLevel(voBuildings.radar_station) == 0 then
									table.insert(loCorePrv.PrvRadarStation, liProvinceId)
								end
							end
						end
					end
				end
				
				-- First make sure the province has Industry (performance reasons done first)
				if lbHasIndustry and ProductionData.ministerTag == loOwnerTag then
					-- Provinces that qualify for Nuclear Reactor and Rocket test sites
					if voBuildings.lbNuclear_reactor or voBuildings.lbRocket_test then
						if (voBuildings.lbRocket_test and loCorePrv.RocketSites < viRocketCap)
						or (voBuildings.lbNuclear_reactor and loCorePrv.ReactorSites < viReactorCap) then
							if not(loProvince:GetCurrentConstructionLevel(voBuildings.rocket_test) > 0) and not(loProvince:GetCurrentConstructionLevel(voBuildings.nuclear_reactor) > 0) then
								table.insert(loCorePrv.PrvForBuilding, liProvinceId)
							end
						end
					end
					
					-- If the Build Industry flag is set figure out provinces that qualify for Industry
					if lbBuildIndustry and voBuildings.lbIndustry then
						if loProvince:GetBuilding(voBuildings.industry):GetMax():Get() < 9
						and not(loProvince:GetCurrentConstructionLevel(voBuildings.industry) > 0) then
							table.insert(loCorePrv.PrvForBuildingIndustry, liProvinceId)
						end
					end
				end
			end
		end
	end
	
	return loCorePrv
end
-- #######################
-- End Building Construction
-- #######################


-- #######################
-- Convoy Building
-- #######################
function ConstructConvoys(viIC)
	if viIC > 0.1 and ProductionData.PortsTotal > 0 then
		local liNeeded = ProductionData.ministerCountry:GetTotalNeededConvoyTransports()
		local liCurrent = ProductionData.ministerCountry:GetTotalConvoyTransports()
		local liConstruction = ProductionData.minister:CountTransportsUnderConstruction()
		local maxSerial = 2

		-- Grab the Convoy Ratios and Calculate Convoys Needed
		local laConvoyRatio = GetBuildRatio("ConvoyRatio")
		local liNeededMultiplier = ((100 + laConvoyRatio[1]) * .01)
		local liLowCap = laConvoyRatio[2]
		local liHighCap = laConvoyRatio[3]
		local liEscortRatio = laConvoyRatio[4]
		
		local liActuallyNeeded = Utils.Round((((liNeeded * liNeededMultiplier) - liCurrent) - liConstruction ))
		local liLowCapNeeded = (liNeeded - (liCurrent + liConstruction)) + liLowCap
		local liHighCapNeeded = (liNeeded - (liCurrent + liConstruction)) + liHighCap
		
		if liLowCapNeeded > liActuallyNeeded then
			liActuallyNeeded = liLowCapNeeded
		elseif liActuallyNeeded > liHighCapNeeded then
			liActuallyNeeded = liHighCapNeeded
		end
		
		-- If their convoy reserves are to low then build smaller serial runs
		if liActuallyNeeded > 100 then maxSerial = 5 end
		
		if liActuallyNeeded > 0 then
			local liCost = ProductionData.ministerCountry:GetConvoyBuildCost():Get()
			local liRequested = math.ceil(liActuallyNeeded / defines.economy.CONVOY_CONSTRUCTION_SIZE)
			viIC = BuildTransportOrEscort(liRequested, maxSerial, false, liCost, viIC)
		end

		-- Now Process Escorts Check
		local liENeeded = 0
		-- Seperate line in case of Ratio of 0
		if liEscortRatio > 0 then 
			liENeeded = math.ceil((liNeeded + liLowCap) / liEscortRatio)
		end
		
		local liECurrent = ProductionData.ministerCountry:GetEscorts()
		local liEConstruction = ProductionData.minister:CountEscortsUnderConstruction()
		local lEActuallyNeeded = liENeeded - (liECurrent + liEConstruction)

		-- If we need escorts lets build them
		if lEActuallyNeeded > 0 then
			local liCost = ProductionData.ministerCountry:GetEscortBuildCost():Get()
			local liRequested = math.ceil(lEActuallyNeeded / defines.economy.CONVOY_CONSTRUCTION_SIZE)
			viIC = BuildTransportOrEscort(liRequested, 5, true, liCost, viIC)
		end 
	end
	
	return viIC
end
--vbConvoyOrEscort = is a boolean (true = escort, false = convoy)
function BuildTransportOrEscort(viNeeded, viMaxSerial, vbConvoyOrEscort, viICCost, viIC)
	while viNeeded > 0 do
		local liSerial = viMaxSerial
		if 	viNeeded < viMaxSerial then liSerial = viNeeded end
		viNeeded = viNeeded - liSerial
		
		if viIC > 0.1 then
			local loCommand = CConstructConvoyCommand(ProductionData.ministerTag, vbConvoyOrEscort, liSerial)
			ProductionData.ministerAI:Post(loCommand)
			viIC = viIC - viICCost
		end
	end
	
	return viIC
end
-- #######################
-- END Convoy Building
-- #######################
	
	--old code of SF and random hoi
	function ConstructBuildings_ports( ai,ministerTag, ministerCountry, ic)
	
		if ministerCountry:GetNumOfPorts() < 5 then

			local naval_base = CBuildingDataBase.GetBuilding("naval_base" )
			local naval_baseCost = ministerCountry:GetBuildCost(naval_base):Get()

			if (ic - naval_baseCost) >= 0 then
				local totalbuilt = 0
				local TotalBeingBuilt = 0
			
				for provinceId in ministerCountry:GetOwnedProvinces() do
					local province = CCurrentGameState.GetProvince(provinceId)
					
					if (province:GetCurrentConstructionLevel(naval_base) > 0) then
						TotalBeingBuilt = TotalBeingBuilt + 1
						
						if TotalBeingBuilt >= 1 then
								break
						end
					end
				end
			
			
				if TotalBeingBuilt < 1 then
					totalbuilt = TotalBeingBuilt
			
					for provinceId in ministerCountry:GetOwnedProvinces() do
						local province = CCurrentGameState.GetProvince(provinceId)
						local provinceBuilding = province:GetBuilding(naval_base)
						
						local constructCommand = CConstructBuildingCommand(ministerTag, naval_base, provinceId, 1 )

						if constructCommand:IsValid() then
							ai:Post( constructCommand )
							totalbuilt = totalbuilt + 1
							ic = ic - naval_baseCost

							if totalbuilt >= 1 or (ic - naval_baseCost) <= 0 then
								break 
							end
						end						
					end
				end
			end
		end
	return ic
 end