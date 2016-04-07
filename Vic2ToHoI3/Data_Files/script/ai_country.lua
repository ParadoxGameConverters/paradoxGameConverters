-- generate a relation value between our country and another
function AI_GenerateNonHistoricalRelation(country, target)
	local desired = 25
	local relation = country:GetRelation(target)
	local targetCountry = target:GetCountry()

	if relation:HasAlliance() then 
		desired = desired + 125
		
		if country:IsAtWar() and targetCountry:IsAtWar() then
			desired = desired + 75
		end
	end

	if relation:HasWar() then
		desired = desired - 200
	end

	if relation:HasTruce() then
		desired = desired - 100
	end

	if country:GetGovernment() == targetCountry:GetGovernment() then
		desired = desired + 10
	end

	if country:GetFaction() == targetCountry:GetFaction() then
		desired = desired + 50
	else
		desired = desired - 25
	end

	return desired
end
