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

	static string getManpower()
	{
		return getInstance()->manpower;
	}

	static string getBasetax()
	{
		return getInstance()->basetax;
	}

	static string getPopulation()
	{
		return getInstance()->population;
	}

	static string getHRETitle()
	{
		return getInstance()->HRETitle;
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
	string	manpower;
	string	basetax;
	string	population;
	string	HRETitle;

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
};

#endif // CONFIGURATION_H_