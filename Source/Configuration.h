#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_


#include <string>
using namespace std;



class Configuration // Singleton
{
public:
	Configuration();

	static string getCK2Path()
	{
		return getInstance()->CK2Path;
	}

	static string getEU3Path()
	{
		return getInstance()->EU3Path;
	}

	static string getProxyMultiplierMethod()
	{
		return getInstance()->proxyMultiplierMethod;
	}

	static string getMultipleProvsMethod()
	{
		return getInstance()->multipleProvsMethod;
	}

	static string getManpower()
	{
		return getInstance()->manpower;
	}

	static string getManpowerblendamount()
	{
		return getInstance()->manpowerblendamount;
	}

	static string getBasetax()
	{
		return getInstance()->basetax;
	}

	static string getBasetaxblendamount()
	{
		return getInstance()->basetaxblendamount;
	}

	static string getPopulation()
	{
		return getInstance()->population;
	}

	static string getPopulationblendamount()
	{
		return getInstance()->populationblendamount;
	}

	static string getHRETitle()
	{
		return getInstance()->HRETitle;
	}

	static string getMergeTitles()
	{
		return getInstance()->mergeTitles;
	}

	static string getVassalScore()
	{
		return getInstance()->vassalScore;
	}

	static string getAdvisors()
	{
		return getInstance()->advisors;
	}

	static string getLeaders()
	{
		return getInstance()->leaders;
	}

	static string getColonists()
	{
		return getInstance()->colonists;
	}

	static string getMerchants()
	{
		return getInstance()->merchants;
	}

	static string getMissionaries()
	{
		return getInstance()->missionaries;
	}

	static string getInflation()
	{
		return getInstance()->inflation;
	}

	static string getColonist_size()
	{
		return getInstance()->colonist_size;
	}

	static string getDifficulty()
	{
		return getInstance()->difficulty;
	}

	static string getAI_aggressiveness()
	{
		return getInstance()->AI_aggressiveness;
	}

	static string getLand_spread()
	{
		return getInstance()->land_spread;
	}

	static string getSea_spread()
	{
		return getInstance()->sea_spread;
	}

	static string getSpies()
	{
		return getInstance()->spies;
	}

	static string getLucky_nations()
	{
		return getInstance()->lucky_nations;
	}


	static int getID()
	{
		return getInstance()->id++;
	}

	static int getArmyID()
	{
		return getInstance()->armyId++;
	}

	static Configuration* getInstance()
	{
		if (instance == NULL)
			instance = new Configuration();
		return instance;
	}

private:
	static Configuration* instance;

	string	CK2Path;
	string	EU3Path;
	string	proxyMultiplierMethod;
	string	multipleProvsMethod;
	string	manpower;
	string	manpowerblendamount;
	string	basetax;
	string	basetaxblendamount;
	string	population;
	string	populationblendamount;
	string	HRETitle;
	string	mergeTitles;
	string	vassalScore;

	string	advisors;
	string	leaders;
	string	colonists;
	string	merchants;
	string	missionaries;
	string	inflation;
	string	colonist_size;
	string	difficulty;
	string	AI_aggressiveness;
	string	land_spread;
	string	sea_spread;
	string	spies;
	string	lucky_nations;

	int		id;
	int		armyId;
};

#endif // CONFIGURATION_H_