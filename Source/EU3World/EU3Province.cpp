#include "EU3Province.h"
#include "EU3History.h"
#include "EU3Advisor.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\Ck2Province.h"
#include "..\CK2World\CK2Character.h"



EU3Province::EU3Province(int _num, Object* obj, date startDate, map< string, vector<string> >& mapSpreadStrings)
{
	num = _num;

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		capital = capitalObj[0]->getLeaf();
	}
	else
	{
		capital = "";
	}

	vector<Object*> tradeGoodObj = obj->getValue("trade_goods");
	if (tradeGoodObj.size() > 0)
	{
		tradeGood = tradeGoodObj[0]->getLeaf().c_str();
	}
	else
	{
		tradeGood = "";
	}

	vector<Object*> baseTaxObj = obj->getValue("base_tax");
	if (baseTaxObj.size() > 0)
	{
		baseTax = atof( baseTaxObj[0]->getLeaf().c_str() );
	}
	else
	{
		baseTax = 0.0f;
	}

	vector<Object*> populationObj = obj->getValue("citysize");
	if (populationObj.size() > 0)
	{
		population = atof( populationObj[0]->getLeaf().c_str() );
	}
	else
	{
		population = 0.0f;
	}

	vector<Object*> manpowerObj = obj->getValue("manpower");
	if (manpowerObj.size() > 0)
	{
		manpower = atoi ( manpowerObj[0]->getLeaf().c_str() );
	}
	else {
		manpower = 0;
	}

	vector<Object*> ownerObj = obj->getValue("owner");
	if (ownerObj.size() > 0)
	{
		owner = ownerObj[0]->getLeaf();
	}
	else
	{
		owner = "";
	}
	srcOwner = NULL;

	cores.clear();
	inHRE = false;

	vector<Object*> discoveredByObj = obj->getValue("discovered_by");
	for (unsigned int i = 0; i < discoveredByObj.size(); i++)
	{
		vector<string> discoverers = mapSpreadStrings[ discoveredByObj[i]->getLeaf() ];
		for (unsigned int j = 0; j < discoverers.size(); j++)
		{
			discoveredBy.push_back( discoverers[j] );
		}
	}

	vector<Object*> cultureObj = obj->getValue("culture");
	if (cultureObj.size() > 0)
	{
		culture = cultureObj[0]->getLeaf();
	}
	else
	{
		culture = "";
	}

	vector<Object*> religionObj = obj->getValue("religion");
	if (religionObj.size() > 0)
	{
		religion = religionObj[0]->getLeaf();
	}
	else
	{
		religion = "";
	}

	history.clear();

	// update based on history
	vector<Object*> objectList = obj->getLeaves();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		string key = objectList[i]->getKey();
		if (key[0] == '1')
		{
			date histDate(key);
			if (histDate <= startDate)
			{
				EU3History* newHistory = new EU3History(histDate);
				vector<Object*> capitalObj = obj->getValue("capital");
				if (capitalObj.size() > 0)
				{
					capital = capitalObj[0]->getLeaf();
					newHistory->capital = capital;
				}

				vector<Object*> tradeGoodObj = obj->getValue("trade_goods");
				if (tradeGoodObj.size() > 0)
				{
					tradeGood = tradeGoodObj[0]->getLeaf().c_str();
					newHistory->tradeGood = tradeGood;
				}

				vector<Object*> populationObj = obj->getValue("citysize");
				if (populationObj.size() > 0)
				{
					population = atof( populationObj[0]->getLeaf().c_str() );
					newHistory->population = population;					
				}

				vector<Object*> manpowerObj = obj->getValue("manpower");
				if (manpowerObj.size() > 0)
				{
					manpower = atoi ( manpowerObj[0]->getLeaf().c_str() );
					newHistory->manpower = manpower;
				}

				vector<Object*> newOwnerObj = objectList[i]->getValue("owner");
				if (newOwnerObj.size() > 0)
				{
					owner = newOwnerObj[0]->getLeaf();
					newHistory->owner = owner;
				}

				vector<Object*> discoveredByObj = obj->getValue("discovered_by");
				for (unsigned int i = 0; i < discoveredByObj.size(); i++)
				{
					vector<string> discoverers = mapSpreadStrings[ discoveredByObj[i]->getLeaf() ];
					for (unsigned int j = 0; j < discoverers.size(); j++)
					{
						newHistory->discoverers.push_back( discoverers[j] );
					}
				}

				vector<Object*> cultureObj = obj->getValue("culture");
				if (cultureObj.size() > 0)
				{
					culture = cultureObj[0]->getLeaf();
					newHistory->culture = culture;
				}

				vector<Object*> religionObj = obj->getValue("religion");
				if (religionObj.size() > 0)
				{
					religion = religionObj[0]->getLeaf();
					newHistory->religion = religion;
				}

				history.push_back(newHistory);
			}
		}
	}

	modifiers.clear();
}


void EU3Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	if (owner != "")
	{
		fprintf(output, "	owner=\"%s\"\n", owner.c_str());
	}
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "	core=\"%s\"\n", cores[i].c_str());
	}
	if (inHRE)
	{
		fprintf(output, "	hre=yes\n");
	}
	if (culture != "")
	{
		fprintf(output, "\tculture=%s\n", culture.c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\treligion=%s\n", religion.c_str());
	}
	if (capital != "")
	{
		fprintf(output, "\tcapital=\"%s\"\n", capital.c_str());
	}
	if (baseTax != 0.0)
	{
		fprintf(output, "\tbase_tax=%f\n", baseTax);
	}
	if (tradeGood != "")
	{
		fprintf(output, "\ttrade_goods=%s\n", tradeGood.c_str());
	}
	if (population != 0.0)
	{
		fprintf(output, "\tcitysize=%f\n", population);
	}
	if (manpower != 0)
	{
		fprintf(output, "\tmanpower=%d\n", manpower);
	}
	fprintf(output, "	history=\n");
	fprintf(output, "	{\n");
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "\t\tadd_core=\"%s\"\n", cores[i].c_str());
	}
	if (owner != "")
	{
		fprintf(output, "\t\towner=\"%s\"\n", owner.c_str());
	}
	if (capital != "")
	{
		fprintf(output, "\t\tcapital=\"%s\"\n", capital.c_str());
	}
	if (baseTax != 0.0)
	{
		fprintf(output, "\t\tbase_tax=%f\n", baseTax);
	}
	if (tradeGood != "")
	{
		fprintf(output, "\t\ttrade_goods=%s\n", tradeGood.c_str());
	}
	if (population != 0.0)
	{
		fprintf(output, "\t\tcitysize=%f\n", population);
	}
	if (manpower != 0)
	{
		fprintf(output, "\t\tmanpower=%d\n", manpower);
	}
	if (culture != "")
	{
		fprintf(output, "\t\tculture=%s\n", culture.c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\t\treligion=%s\n", religion.c_str());
	}
	if (inHRE)
	{
		fprintf(output, "\t\thre=yes\n");
	}
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "	}\n");
	fprintf(output, "	discovery_dates={9999.1.1 9999.1.1 1458.4.30 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "	discovery_religion_dates={9999.1.1 1458.4.30 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "	discovered_by={ ");
	for (unsigned int i = 0; i < discoveredBy.size(); i++)
	{
		fprintf(output, "%s ", discoveredBy[i].c_str());
	}
	fprintf(output, "	}\n");
	for (unsigned int i = 0; i < modifiers.size(); i++)
	{
		fprintf(output, "\tmodifier=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tmodifier=\"%s\"\n", modifiers[i].c_str());
		fprintf(output, "\t}\n");
	}
	fprintf(output, "}\n");
}


void EU3Province::convert(int _num, bool _inHRE, const vector<string> _discoveredBy)
{
	num				= _num;
	//capital	-- leave it as it is from the history file
	owner				= "";
	cores.clear();
	inHRE				= _inHRE;
	discoveredBy	= _discoveredBy;
	history.clear();
	culture			= "";
}


void EU3Province::addAdvisor(EU3Advisor* advisor)
{
	EU3History* newHistory = new EU3History(advisor->getStartDate());
	newHistory->advisor = advisor;
	history.push_back(newHistory);
}


string EU3Province::determineEU3Culture(const cultureMapping& cultureMap, const CK2Province* srcProvince)
{
	string CK2Culture = srcProvince->getCulture();
	if (CK2Culture[0] == '"')
	{
		CK2Culture = CK2Culture.substr(1, CK2Culture.size() - 2);
	}
	for (cultureMapping::const_iterator itr = cultureMap.begin(); itr < cultureMap.end(); itr++)
	{
		if (itr->srcCulture != CK2Culture)
		{
			continue;
		}

		bool matchConditions = true;
		for (vector<distinguisher>::const_iterator DTItr = itr->distinguishers.begin(); DTItr < itr->distinguishers.end(); DTItr++)
		{
			switch (DTItr->first)
			{
				case DTDeJure:
					{
						bool subCondition = false;
						CK2Title* title = srcProvince->getBaronies()[0]->getTitle()->getLiege();
						while(title != NULL)
						{
							if (title->getTitleString() == DTItr->second)
							{
								subCondition = true;
								break;
							}
							title = title->getDeJureLiege();
						}
						matchConditions = subCondition;
					}
					break;
				case DTKingdomCulture:
					{
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							kingdomTitle = kingdomTitle->getLiege();
						}
						if (kingdomTitle->getHolder()->getCulture() != DTItr->second)
						{
							matchConditions = false;
						}
					}
					break;
				case DTReligion:
					if (srcProvince->getReligion() != DTItr->second)
					{
						matchConditions = false;
					}
					break;
				case DTHREMember:
					{
						bool subCondition = false;
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							if (kingdomTitle->getLiegeString() == "e_hre")
							{
								subCondition = true;
								break;
							}
							kingdomTitle = kingdomTitle->getLiege();
						}
						matchConditions = subCondition;
					}
					break;
			}
		}
		if (!matchConditions)
		{
			continue;
		}
		return itr->dstCulture;
	}

	return "";
}


void EU3Province::determineCulture(const cultureMapping& cultureMap, const vector<CK2Province*>& srcProvinces, const vector<CK2Barony*> baronies)
{
	map<string, double> cultureCounts;
	map<string, double> cultureCounts2;
	map<string, double> cultureCounts3;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		double popProxy		= 0.0f;
		double basetaxProxy	= 0.0f;
		double manpowerProxy	= 0.0f;
		vector<CK2Barony*> baronies = (*provItr)->getBaronies();
		for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			popProxy			+= (*baronyItr)->getPopProxy();
			basetaxProxy	+= (*baronyItr)->getBaseTaxProxy();
			manpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}

		string EU3Culture = determineEU3Culture(cultureMap, *provItr);
		map<string, double>::iterator cultureItr = cultureCounts.find(EU3Culture);
		if (cultureItr == cultureCounts.end())
		{
			cultureCounts[EU3Culture] = popProxy;
		}
		else
		{
			cultureItr->second += popProxy;
		}

		cultureItr = cultureCounts2.find(EU3Culture);
		if (cultureItr == cultureCounts2.end())
		{
			cultureCounts2[EU3Culture] = basetaxProxy;
		}
		else
		{
			cultureItr->second += basetaxProxy;
		}

		cultureItr = cultureCounts3.find(EU3Culture);
		if (cultureItr == cultureCounts3.end())
		{
			cultureCounts3[EU3Culture] = manpowerProxy;
		}
		else
		{
			cultureItr->second += manpowerProxy;
		}
	}

	string			topCulture		= "";
	double			highestCount	= 0;
	vector<string>	tiedCultures;
	vector<string>	tiedCultures2;
	bool				tie;
	for (map<string, double>::iterator countsItr = cultureCounts.begin(); countsItr != cultureCounts.end(); countsItr++)
	{
		if (countsItr->second > highestCount)
		{
			topCulture		= countsItr->first;
			highestCount	= countsItr->second;
			tiedCultures.clear();
			tiedCultures.push_back(countsItr->first);
			tie = false;
		}
		else if (countsItr->second == highestCount)
		{
			tiedCultures.push_back(countsItr->first);
			tie = true;
		}
	}

	if (tie == true)
	{
		topCulture		= "";
		highestCount	= 0;
		vector<string>	tiedCultures2;
		for (map<string, double>::iterator countsItr = cultureCounts2.begin(); countsItr != cultureCounts2.end(); countsItr++)
		{
			for (vector<string>::iterator cultureItr = tiedCultures.begin(); cultureItr != tiedCultures.end(); cultureItr++)
			{
				if (countsItr->first == *cultureItr)
				{
					if (countsItr->second > highestCount)
					{
						topCulture		= countsItr->first;
						highestCount	= countsItr->second;
						tiedCultures2.clear();
						tiedCultures2.push_back(countsItr->first);
						tie = false;
					}
					else if (countsItr->second == highestCount)
					{
						tiedCultures2.push_back(countsItr->first);
						tie = true;
					}
					break;
				}
			}
		}
	}

	if (tie == true)
	{
		topCulture		= "";
		highestCount	= 0;
		for (map<string, double>::iterator countsItr = cultureCounts3.begin(); countsItr != cultureCounts3.end(); countsItr++)
		{
			for (vector<string>::iterator cultureItr = tiedCultures2.begin(); cultureItr != tiedCultures2.end(); cultureItr++)
			{
				if (countsItr->first == *cultureItr)
				{
					if (countsItr->second > highestCount)
					{
						topCulture		= countsItr->first;
						highestCount	= countsItr->second;
						tie = false;
					}
					else if (countsItr->second == highestCount)
					{
						tie = true;
					}
					break;
				}
			}
		}
	}

	if (tie == true)
	{
		log("\tWarning: could not decide on culture for EU3 province %d due to ties.\n", num);
	}

	culture = topCulture;
}


void EU3Province::determineReligion(const religionMapping& religionMap, const vector<CK2Province*>& srcProvinces)
{

	map<string, double> religionCounts;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		double popProxy = 0.0f;
		vector<CK2Barony*> baronies = (*provItr)->getBaronies();
		for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			popProxy += (*baronyItr)->getPopProxy();
		}


		string _religion = (*provItr)->getReligion();
		map<string, double>::iterator countsItr = religionCounts.find(_religion);
		if (countsItr != religionCounts.end())
		{
			countsItr->second += popProxy;
		}
		else
		{
			religionCounts[_religion] = popProxy;
		}
	}

	string			topReligion;
	double			topCount		= 0;
	bool				tie			= false;
	vector<string>	tiedReligions;
	for (map<string, double>::const_iterator countsItr = religionCounts.begin(); countsItr != religionCounts.end(); countsItr++)
	{
		if (countsItr->second > topCount)
		{
			topReligion	= countsItr->first;
			topCount		= countsItr->second;
			tie			= false;
			tiedReligions.clear();
			tiedReligions.push_back(countsItr->first);
		}
		else if (countsItr->second == topCount)
		{
			tie			= true;
			tiedReligions.push_back(countsItr->first);
		}
	}

	if (tie)
	{
		log("\tError: tied for religions in province %d, and no method to distinguish.\n", num);
	}


	const CK2Title* rulerTitle = srcOwner;
	if (srcOwner != NULL)
	{
		while (!rulerTitle->isIndependent())
		{
			rulerTitle = rulerTitle->getLiege();
		}
		if ( rulerTitle->getHolder()->getReligion() != topReligion )
		{
			if ( religionMap.find(rulerTitle->getHolder()->getReligion())->second == religionMap.find(topReligion)->second )
			{
				modifiers.push_back("heresy");
			}
		}
	}

	religionMapping::const_iterator religionItr = religionMap.find(topReligion);
	if (religionItr != religionMap.end())
	{
		religion = religionItr->second;
	}
	else if (topReligion == "")
	{
	}
	else
	{
		log("\tError: could not map religion %s to any EU3 religions (province %d: %s)\n", topReligion.c_str(), num, capital.c_str());
	}
}


void EU3Province::setManpower(double _manpower)
{
	manpower = (int)(2 * _manpower);
	if ( manpower % 2 == 0 )
	{
		manpower /= 2;
	}
	else
	{
		manpower /= 2;
		manpower++;
	}
}