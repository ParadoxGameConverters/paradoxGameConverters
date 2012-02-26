#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <vector>
#include "Date.h"

using namespace std;

class Configuration // Singleton
{
public:
	Configuration();

	static date getStartDate()
	{
		return getInstance()->startDate;
	}

	static string getHoI3Path()
	{
		return getInstance()->HoI3Path;
	}

	static string getV2Path()
	{
		return getInstance()->V2Path;
	}

	static string getHoI3Gametype()
	{
		return getInstance()->HoI3gametype;
	}

	static string getV2Gametype()
	{
		return getInstance()->V2gametype;
	}

	static string getRemovetype()
	{
		return getInstance()->removetype;
	}

	static string getFactionLeaderAlgo()
	{
		return getInstance()->factionLeaderAlgorithm;
	}

	static vector<string> getManualAxisFaction()
	{
		return getInstance()->manualAxisFaction;
	}

	static vector<string> getManualAlliesFaction()
	{
		return getInstance()->manualAlliesFaction;
	}

	static vector<string> getManualCominternFaction()
	{
		return getInstance()->manualCominternFaction;
	}

	static double getMoneyFactor()
	{
		return getInstance()->moneyFactor;
	}

	static double getMinInfra()
	{
		return getInstance()->minInfra;
	}

	static Configuration* getInstance()
	{
		if (instance == NULL)
			instance = new Configuration();
		return instance;
	}

private:
	static Configuration* instance;

	date startDate;
	string HoI3Path;
	string V2Path;
	string HoI3gametype;
	string V2gametype;
	string removetype;
	string factionLeaderAlgorithm;
	double moneyFactor;
	double minInfra;
	vector<string> manualAxisFaction;
	vector<string> manualAlliesFaction;
	vector<string> manualCominternFaction;
};

#endif // CONFIGURATION_H_