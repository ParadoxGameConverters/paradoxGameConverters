#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Log.h"

Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	printf("Reading configuration file.\n");
	log("Reading configuration file.\n");

	vector<Object*> obj = doParseFile("configuration.txt")->getValue("configuration");
	if (obj.size() != 1)
	{
		printf("Configuration file must contain exactly one configuration section.");
		log("Configuration file must contain exactly one configuration section.");
		exit (-2);
	}

	V2Path = obj[0]->getLeaf("v2directory");
	HoI3Path = obj[0]->getLeaf("hoi3directory");
	startDate = date(obj[0]->getLeaf("start_date"));
	removetype	= obj[0]->getLeaf("removetype");
	V2gametype = obj[0]->getLeaf("v2gametype");
	HoI3gametype = obj[0]->getLeaf("hoi3gametype");
	moneyFactor = atof(obj[0]->getLeaf("money_factor").c_str());
	factionLeaderAlgorithm = obj[0]->getLeaf("faction_leader");
	minInfra = atof(obj[0]->getLeaf("min_infrastructure").c_str());
	if (factionLeaderAlgorithm == "manual")
	{
		vector<Object*> factionObj = obj[0]->getValue("axis_faction");
		if (factionObj.size() > 0)
		{
			manualAxisFaction = factionObj[0]->getTokens();
		}
		factionObj = obj[0]->getValue("allies_faction");
		if (factionObj.size() > 0)
		{
			manualAlliesFaction = factionObj[0]->getTokens();
		}
		factionObj = obj[0]->getValue("comintern_faction");
		if (factionObj.size() > 0)
		{
			manualCominternFaction = factionObj[0]->getTokens();
		}
	}
}