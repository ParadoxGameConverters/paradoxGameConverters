/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/


/*Copyright (c) 2010 The EU3 to V2 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



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

	static string getCK2ModPath()
	{
		return getInstance()->CK2ModPath;
	}

	static string getTechGroupMethod()
	{
		return getInstance()->techGroupMethod;
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

	static string getUseConverterMod()
	{
		return getInstance()->useConverterMod;
	}

	static string getCK2Mod()
	{
		return getInstance()->CK2Mod;
	}

	static string getModPath()
	{
		return getInstance()->modPath;
	}

	static void setModPath(string _modPath)
	{
		getInstance()->modPath = _modPath;
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
	string	CK2ModPath;

	string	techGroupMethod;

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

	string	useConverterMod;
	string	CK2Mod;
	string	modPath;

	int		id;
	int		armyId;
};

#endif // CONFIGURATION_H_