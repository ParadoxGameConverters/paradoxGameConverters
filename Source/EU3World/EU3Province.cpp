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

	vector<Object*> ownerObj = obj->getValue("owner");
	if (ownerObj.size() > 0)
	{
		owner = ownerObj[0]->getLeaf();
	}
	else
	{
		owner = "";
	}

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
				vector<Object*> capitalObj = obj->getValue("capital");
				if (capitalObj.size() > 0)
				{
					capital = capitalObj[0]->getLeaf();
					EU3History* newHistory = new EU3History(histDate, NULL, NULL, NULL, NULL, capital, "", "", "", vector<string>());
					history.push_back(newHistory);
				}

				vector<Object*> newOwnerObj = objectList[i]->getValue("owner");
				if (newOwnerObj.size() > 0)
				{
					owner = newOwnerObj[0]->getLeaf();
					EU3History* newHistory = new EU3History(histDate, NULL, NULL, NULL, NULL, "", owner, "", "", vector<string>());
					history.push_back(newHistory);
				}

				vector<Object*> discoveredByObj = obj->getValue("discovered_by");
				for (unsigned int i = 0; i < discoveredByObj.size(); i++)
				{
					vector<string> discoverers = mapSpreadStrings[ discoveredByObj[i]->getLeaf() ];
					for (unsigned int j = 0; j < discoverers.size(); j++)
					{
						discoveredBy.push_back( discoverers[j] );
					}
					EU3History* newHistory = new EU3History(histDate, NULL, NULL, NULL, NULL, "", "", "", "", discoverers);
					history.push_back(newHistory);
				}

				vector<Object*> cultureObj = obj->getValue("culture");
				if (cultureObj.size() > 0)
				{
					culture = cultureObj[0]->getLeaf();
					EU3History* newHistory = new EU3History(histDate, NULL, NULL, NULL, NULL, "", "", "", culture, vector<string>());
					history.push_back(newHistory);
				}

				vector<Object*> religionObj = obj->getValue("religion");
				if (religionObj.size() > 0)
				{
					religion = religionObj[0]->getLeaf();
					EU3History* newHistory = new EU3History(histDate, NULL, NULL, NULL, NULL, "", "", "", religion, vector<string>());
				}
			}
		}
	}
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


void EU3Province::addAdvisor(EU3Advisor* newAdvisor)
{
	EU3History* newHistory = new EU3History(newAdvisor->getStartDate(), NULL, NULL, NULL, newAdvisor, "", "", "", "", vector<string>());
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
	map<string, int> cultureCounts;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		string EU3Culture = determineEU3Culture(cultureMap, *provItr);
		map<string, int>::iterator cultureItr = cultureCounts.find(EU3Culture);
		if (cultureItr == cultureCounts.end())
		{
			cultureCounts[EU3Culture] = 1;
		}
		else
		{
			cultureItr->second++;
		}
	}

	string			topCulture		= "";
	int				highestCount	= 0;
	vector<string>	tiedCultures;
	bool				tie;
	for (map<string, int>::iterator countsItr = cultureCounts.begin(); countsItr != cultureCounts.end(); countsItr++)
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

	//if (tie == true)
	//{
	//	log("\tCulture tie in province %d, using barons to distinguish.\n", num);
	//	cultureCounts.clear();
	//	for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr < baronies.end(); baronyItr++)
	//	{
	//		string EU3Culture = determineEU3Culture( cultureMap, (*baronyItr)->getTitle()->getHolder()->getCulture() );
	//		bool isATiedCulture = false;
	//		for (vector<string>::iterator tiedItr = tiedCultures.begin(); tiedItr < tiedCultures.end(); tiedItr++)
	//		{
	//			if (EU3Culture == *tiedItr)
	//			{
	//				map<string, int>::iterator cultureItr = cultureCounts.find(EU3Culture);
	//				if (cultureItr == cultureCounts.end())
	//				{
	//					cultureCounts[EU3Culture] = 1;
	//				}
	//				else
	//				{
	//					cultureItr->second++;
	//				}
	//			}
	//		}
	//	}

	//	topCulture		= "";
	//	highestCount	= 0;
	//	tiedCultures.clear();
	//	for (map<string, int>::iterator countsItr = cultureCounts.begin(); countsItr != cultureCounts.end(); countsItr++)
	//	{
	//		if (countsItr->second > highestCount)
	//		{
	//			topCulture		= countsItr->first;
	//			highestCount	= countsItr->second;
	//			tiedCultures.clear();
	//			tie = false;
	//		}
	//		else if (countsItr->second == highestCount)
	//		{
	//			if (tiedCultures.size() == 0)
	//			{
	//				tiedCultures.push_back(topCulture);
	//			}
	//			tiedCultures.push_back(countsItr->first);
	//			tie = true;
	//		}
	//	}
	//}

	if (tie == true)
	{
		log("\tError: could not decide on culture for EU3 province %d due to ties.\n", num);
	}

	culture = topCulture;
}


void EU3Province::determineReligion(const religionMapping& religionMap, const vector<CK2Province*>& srcProvinces)
{
	map<string, int> religionCounts;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		religionMapping::const_iterator religionItr = religionMap.find( (*provItr)->getReligion() );
		if (religionItr != religionMap.end())
		{
			map<string, int>::iterator countsItr = religionCounts.find(religionItr->second);
			if (countsItr != religionCounts.end())
			{
				countsItr->second++;
			}
			else
			{
				religionCounts[religionItr->second] = 1;
			}
		}
		else
		{
			log("\tError: Could not map CK2 religion %s to any EU3 religion.\n");
		}
	}

	string			topReligion	= "";
	int				topCount		= 0;
	bool				tie			= false;
	vector<string>	tiedReligions;
	for (map<string, int>::const_iterator countsItr = religionCounts.begin(); countsItr != religionCounts.end(); countsItr++)
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
		log("\tError: tied for religions in province %d.\n", num);
	}

	religion = topReligion;
}