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

	static string getGametype()
	{
		return getInstance()->gametype;
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
	string gametype;
	string factionLeaderAlgorithm;
	vector<string> manualAxisFaction;
	vector<string> manualAlliesFaction;
	vector<string> manualCominternFaction;
};

#endif // CONFIGURATION_H_