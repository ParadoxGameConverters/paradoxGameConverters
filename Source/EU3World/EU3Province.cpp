#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3History.h"
#include "EU3Advisor.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\Ck2Province.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Religion.h"
#include <algorithm>


EU3Province::EU3Province(int _num, Object* obj, date _startDate)
{
	srcProvinces.clear();
	srcProvinceNums.clear();
	startDate	= _startDate;
	num			= _num;

	vector<Object*> capitalObj = obj->getValue("capital");
	if (capitalObj.size() > 0)
	{
		land		= true;
		capital	= capitalObj[0]->getLeaf();
	}
	else
	{
		land		= false;
		capital	= "";
	}
	coastal = false;

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
	else
	{
		manpower = 0;
	}

	buildings.clear();
	vector<Object*> fortObj = obj->getValue("fort2");
	if (fortObj.size() > 0)
	{
		buildings.push_back("fort2");
	}
	else
	{
		vector<Object*> fortObj = obj->getValue("fort1");
		if (fortObj.size() > 0)
		{
			buildings.push_back("fort1");
		}
	}

	owner = NULL;
	vector<Object*> ownerObj = obj->getValue("owner");
	if (ownerObj.size() > 0)
	{
		ownerStr = ownerObj[0]->getLeaf();
	}
	else
	{
		ownerStr = "";
	}
	srcOwner = NULL;

	cores.clear();
	coreStrings.clear();
	vector<Object*> coreObj = obj->getValue("add_core");
	for (vector<Object*>::iterator itr = coreObj.begin(); itr != coreObj.end(); itr++)
	{
		coreStrings.push_back( (*itr)->getLeaf() );
	}
	inHRE = false;

	rawDiscoverers.clear();
	vector<Object*> discoveredByObj = obj->getValue("discovered_by");
	for (unsigned int i = 0; i < discoveredByObj.size(); i++)
	{
		rawDiscoverers.push_back(discoveredByObj[i]->getLeaf());
	}
	discoveredBy.clear();

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
	modifiers.clear();

	cot = false;
	vector<Object*> cotObj = obj->getValue("cot");
	if (cotObj.size() > 0)
	{
		if (cotObj[0]->getLeaf() == "yes")
		{
			cot = true;
		}
	}
	tradeStation = false;

	continent		= "";
	sameContinent	= false;
	landConnection	= false;

	popUnits	= 0.0f;
	supply	= 0.0f;
	demands.clear();
	armyHere	= false;

	vector<Object*> nativeSizeObj = obj->getValue("native_size");
	if (nativeSizeObj.size() > 0)
	{
		nativeSize = atof(nativeSizeObj[0]->getLeaf().c_str());
	}
	else
	{
		nativeSize = 0.0f;
	}

	vector<Object*> nativeFerocityObj = obj->getValue("native_ferocity");
	if (nativeFerocityObj.size() > 0)
	{
		nativeFerocity = atoi(nativeFerocityObj[0]->getLeaf().c_str());
	}
	else
	{
		nativeFerocity = 0;
	}

	vector<Object*> nativeHostilityObj = obj->getValue("native_hostileness");
	if (nativeHostilityObj.size() > 0)
	{
		nativeHostility = atoi(nativeHostilityObj[0]->getLeaf().c_str());
	}
	else
	{
		nativeHostility = 0;
	}

	numRegiments	= 0;
	numShips			= 0;

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
				vector<Object*> capitalObj = objectList[i]->getValue("capital");
				if (capitalObj.size() > 0)
				{
					capital = capitalObj[0]->getLeaf();
					newHistory->capital = capital;
				}

				vector<Object*> tradeGoodObj = objectList[i]->getValue("trade_goods");
				if (tradeGoodObj.size() > 0)
				{
					tradeGood = tradeGoodObj[0]->getLeaf().c_str();
					newHistory->tradeGood = tradeGood;
				}

				vector<Object*> populationObj = objectList[i]->getValue("citysize");
				if (populationObj.size() > 0)
				{
					population = atof( populationObj[0]->getLeaf().c_str() );
					newHistory->population = population;					
				}

				vector<Object*> manpowerObj = objectList[i]->getValue("manpower");
				if (manpowerObj.size() > 0)
				{
					manpower = atoi ( manpowerObj[0]->getLeaf().c_str() );
					newHistory->manpower = manpower;
				}

				vector<Object*> fortObj = objectList[i]->getValue("fort2");
				if (fortObj.size() > 0)
				{
					buildings.clear();
					buildings.push_back("fort2");
				}
				else
				{
					vector<Object*> fortObj = objectList[i]->getValue("fort1");
					if (fortObj.size() > 0)
					{
						buildings.push_back("fort1");
					}
				}

				vector<Object*> newOwnerObj = objectList[i]->getValue("owner");
				if (newOwnerObj.size() > 0)
				{
					ownerStr = newOwnerObj[0]->getLeaf();
					newHistory->owner = ownerStr;
				}

				coreObj = objectList[i]->getValue("add_core");
				for (vector<Object*>::iterator itr = coreObj.begin(); itr != coreObj.end(); itr++)
				{
					coreStrings.push_back( (*itr)->getLeaf() );
					newHistory->add_core = (*itr)->getLeaf();
				}

				coreObj = objectList[i]->getValue("remove_core");
				for (vector<Object*>::iterator itr = coreObj.begin(); itr != coreObj.end(); itr++)
				{
					for (vector<string>::iterator coreItr = coreStrings.begin(); coreItr != coreStrings.end(); coreItr++)
					{
						if (*coreItr == (*itr)->getLeaf())
						{
							coreStrings.erase(coreItr);
							newHistory->remove_core = (*itr)->getLeaf();
							break;
						}
					}
				}

				vector<Object*> discoveredByObj = obj->getValue("discovered_by");
				for (unsigned int j = 0; j < discoveredByObj.size(); j++)
				{
					for (unsigned int j = 0; j < discoveredByObj.size(); j++)
					{
						rawDiscoverers.push_back(discoveredByObj[j]->getLeaf());
					}
					for (unsigned int j = 0; j < rawDiscoverers.size(); j++)
					{
						newHistory->discoverers.push_back( rawDiscoverers[j] );
					}
				}

				vector<Object*> cultureObj = objectList[i]->getValue("culture");
				if (cultureObj.size() > 0)
				{
					culture = cultureObj[0]->getLeaf();
					newHistory->culture = culture;
				}

				vector<Object*> religionObj = objectList[i]->getValue("religion");
				if (religionObj.size() > 0)
				{
					religion = religionObj[0]->getLeaf();
					newHistory->religion = religion;
				}

				vector<Object*> cotObj = objectList[i]->getValue("cot");
				if (cotObj.size() > 0)
				{
					if (cotObj[0]->getLeaf() == "yes")
					{
						cot = true;
					}
				}

				history.push_back(newHistory);
			}
		}
	}
}


// get UNIQUE tags from all cores
vector<string> EU3Province::getCoreTags() const
{
	vector<string> out;
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		string tag = cores[i]->getTag();
		if (!tag.empty())
		{
			vector<string>::const_iterator itr = find(out.begin(), out.end(), tag);
			if (itr == out.end())
				out.push_back(tag);
		}
	}
	return out;
}


void EU3Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	if (ownerStr != "")
	{
		fprintf(output, "\towner=\"%s\"\n", ownerStr.c_str());
	}
	else if (owner != NULL)
	{
		fprintf(output, "\towner=\"%s\"\n", owner->getTag().c_str());
	}
	vector<string> coreTags = getCoreTags();
	for (vector<string>::iterator itr = coreTags.begin(); itr != coreTags.end(); ++itr)
	{
		fprintf(output, "\tcore=\"%s\"\n", itr->c_str());
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
	if (population != 0.0)
	{
		fprintf(output, "\tcitysize=%f\n", population);
	}
	if ((ownerStr != "") || (owner != NULL))
	{
		fprintf(output, "\tgarrison=1000.000\n");
	}
	if (baseTax != 0.0)
	{
		fprintf(output, "\tbase_tax=%f\n", baseTax);
	}
	if (manpower != 0)
	{
		fprintf(output, "\tmanpower=%d\n", manpower);
	}
	if (inHRE)
	{
		fprintf(output, "\thre=yes\n");
	}
	if (tradeGood != "")
	{
		fprintf(output, "\ttrade_goods=%s\n", tradeGood.c_str());
	}
	for (vector<string>::iterator i = buildings.begin(); i != buildings.end(); i++)
	{
		fprintf(output, "\t%s=yes\n", (*i).c_str());
	}
	if (nativeSize > 0.0)
	{
		fprintf(output, "\tnative_size=%f\n", nativeSize);
	}
	if (nativeFerocity > 0)
	{
		fprintf(output, "\tnative_ferocity=%d\n", nativeFerocity);
	}
	if (nativeHostility > 0)
	{
		fprintf(output, "\tnative_hostileness=%d\n", nativeHostility);
	}
	fprintf(output, "\thistory=\n");
	fprintf(output, "\t{\n");
	for (vector<string>::iterator itr = coreTags.begin(); itr != coreTags.end(); ++itr)
	{
		fprintf(output, "\t\tadd_core=\"%s\"\n", itr->c_str());
	}
	if (ownerStr != "")
	{
		fprintf(output, "\t\towner=\"%s\"\n", ownerStr.c_str());
	}
	else if (owner != NULL)
	{
		fprintf(output, "\t\towner=\"%s\"\n", owner->getTag().c_str());
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
	if (nativeSize > 0.0)
	{
		fprintf(output, "\t\tnative_size=%f\n", nativeSize);
	}
	if (nativeFerocity > 0)
	{
		fprintf(output, "\t\tnative_ferocity=%d\n", nativeFerocity);
	}
	if (nativeHostility > 0)
	{
		fprintf(output, "\t\tnative_hostileness=%d\n", nativeHostility);
	}
	std::sort(history.begin(), history.end(), [](EU3History* a, EU3History* b) { return a->when < b->when; } ); // by date
	for (unsigned int i = 0; i < history.size(); i++)
	{
		history[i]->output(output);
	}
	fprintf(output, "\t\t%s=\n", startDate.toString().c_str());
	fprintf(output, "\t\t{\n");
	for(unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "\t\t\tadd_core=\"%s\"\n", cores[i]->getTag().c_str());
	}
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t}\n");
	fprintf(output, "\tdiscovery_dates={9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "\tdiscovery_religion_dates={9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 9999.1.1 }\n");
	fprintf(output, "\tdiscovered_by={ ");
	for (unsigned int i = 0; i < discoveredBy.size(); i++)
	{
		fprintf(output, "%s ", discoveredBy[i].c_str());
	}
	fprintf(output, "\t}\n");
	for (unsigned int i = 0; i < modifiers.size(); i++)
	{
		fprintf(output, "\tmodifier=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tmodifier=\"%s\"\n", modifiers[i].c_str());
		fprintf(output, "\t}\n");
	}
	fprintf(output, "}\n");
}


void EU3Province::convert(int _num, bool _inHRE, const vector<CK2Province*>& _srcProvinces, const vector<int>& _srcProvinceNums, vector<EU3Country*> _cores)
{
	srcProvinces = _srcProvinces;
	srcProvinceNums = _srcProvinceNums;

	num				= _num;
	buildings.clear();
	ownerStr			= "";
	cores				= _cores;
	inHRE				= _inHRE;
	discoveredBy.clear();
	history.clear();
	culture			= "";
	tradeStation	= false;

	for (vector<CK2Province*>::iterator provItr = srcProvinces.begin(); provItr != srcProvinces.end(); provItr++)
	{
		if ((*provItr)->hasTradePost())
		{
			modifiers.push_back("league_kontor");
			tradeStation = true;
			break;
		}
	}
}


void EU3Province::addAdvisor(EU3Advisor* advisor)
{
	EU3History* newHistory = new EU3History(advisor->getStartDate());
	newHistory->advisor = advisor;
	history.push_back(newHistory);
}


void EU3Province::determineCulture(const cultureMapping& cultureMap, const vector<CK2Province*>& srcProvinces, const vector<CK2Barony*> baronies)
{
	map<string, double> cultureCounts;
	map<string, double> cultureCounts2;
	map<string, double> cultureCounts3;
	bool hadSrc = false;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		double popProxy		= 0.0f;
		double basetaxProxy	= 0.0f;
		double manpowerProxy	= 0.0f;
		vector<CK2Barony*> baronies = (*provItr)->getBaronies();
		for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			hadSrc			=  true;
			popProxy			+= (*baronyItr)->getPopProxy();
			basetaxProxy	+= (*baronyItr)->getBaseTaxProxy();
			manpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}

		string EU3Culture = determineEU3Culture((*provItr)->getCulture(), cultureMap, *provItr);
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

	if (hadSrc)
	{
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
			log("\tWarning: could not decide on culture for EU3 province %d due to ties. %s arbitrarily assigned.\n", num, topCulture.c_str());
		}

		culture = topCulture;
	}
}


void EU3Province::determineReligion(const religionMapping& religionMap, const vector<CK2Province*>& srcProvinces)
{

	map<CK2Religion*, double> religionCounts;
	map<CK2Religion*, double> religionCounts2;
	map<CK2Religion*, double> religionCounts3;
	for (vector<CK2Province*>::const_iterator provItr = srcProvinces.begin(); provItr < srcProvinces.end(); provItr++)
	{
		double	popProxy			= 0.0f;
		double	baseTaxProxy	= 0.0f;
		double	manpowerProxy	= 0.0f;
		vector<CK2Barony*> baronies = (*provItr)->getBaronies();
		for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			popProxy			+= (*baronyItr)->getPopProxy();
			baseTaxProxy	+= (*baronyItr)->getBaseTaxProxy();
			manpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}


		CK2Religion* _religion = (*provItr)->getReligion();
		map<CK2Religion*, double>::iterator countsItr = religionCounts.find(_religion);
		if (countsItr != religionCounts.end())
		{
			countsItr->second += popProxy;
		}
		else
		{
			religionCounts[_religion] = popProxy;
		}

		countsItr = religionCounts2.find(_religion);
		if (countsItr != religionCounts2.end())
		{
			countsItr->second += baseTaxProxy;
		}
		else
		{
			religionCounts2[_religion] = baseTaxProxy;
		}

		countsItr = religionCounts3.find(_religion);
		if (countsItr != religionCounts3.end())
		{
			countsItr->second += manpowerProxy;
		}
		else
		{
			religionCounts3[_religion] = manpowerProxy;
		}
	}

	CK2Religion*		topReligion = NULL;
	double			topCount		= 0;
	bool				tie			= false;
	vector<CK2Religion*>	tiedReligions;
	for (map<CK2Religion*, double>::const_iterator countsItr = religionCounts.begin(); countsItr != religionCounts.end(); countsItr++)
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

	vector<CK2Religion*>	tiedReligions2;
	if (tie == true)
	{
		topReligion	= NULL;
		topCount		= 0;
		for (map<CK2Religion*, double>::iterator countsItr = religionCounts2.begin(); countsItr != religionCounts2.end(); countsItr++)
		{
			for (vector<CK2Religion*>::iterator cultureItr = tiedReligions.begin(); cultureItr != tiedReligions.end(); cultureItr++)
			{
				if (countsItr->first == *cultureItr)
				{
					if (countsItr->second > topCount)
					{
						topReligion	= countsItr->first;
						topCount		= countsItr->second;
						tiedReligions2.clear();
						tiedReligions2.push_back(countsItr->first);
						tie = false;
					}
					else if (countsItr->second == topCount)
					{
						tiedReligions2.push_back(countsItr->first);
						tie = true;
					}
					break;
				}
			}
		}
	}

	if (tie == true)
	{
		topReligion	= NULL;
		topCount		= 0;
		for (map<CK2Religion*, double>::iterator countsItr = religionCounts3.begin(); countsItr != religionCounts3.end(); countsItr++)
		{
			for (vector<CK2Religion*>::iterator religionItr = tiedReligions2.begin(); religionItr != tiedReligions2.end(); religionItr++)
			{
				if (countsItr->first == *religionItr)
				{
					if (countsItr->second > topCount)
					{
						topReligion	= countsItr->first;
						topCount		= countsItr->second;
						tie = false;
					}
					else if (countsItr->second == topCount)
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
		log("\tWarning: could not decide on religion for EU3 province %d due to ties. %s arbitrarily assigned.\n", num, topReligion->getName().c_str());
	}

	if (topReligion == NULL)
	{
		// do nothing?
	}
	else
	{
		religionMapping::const_iterator religionItr = religionMap.find(topReligion->getName());
		if (religionItr != religionMap.end())
		{
			religion = religionItr->second;
			const CK2Title* rulerTitle = srcOwner;
			if (srcOwner != NULL)
			{
				while (!rulerTitle->isIndependent())
				{
					rulerTitle = rulerTitle->getLiege();
				}
				const CK2Character* ruler = rulerTitle->getLastHolder();
				if ( ruler->getReligion()->isHereticTo(topReligion) )
				{
					modifiers.push_back("heresy");
				}
			}
		}
		else
		{
			log("\tError: could not map religion %s to any EU3 religions (province %d: %s)\n", topReligion->getName().c_str(), num, capital.c_str());
		}
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


void EU3Province::determinePopUnits()
{
	if (population <= 1000)
	{
		popUnits = 0.1 * int(population / 100);
	}
	else
	{
		popUnits = 0.99 + population / 100000;
		if (popUnits > 2)
		{
			popUnits = 2;
		}
		popUnits += baseTax / 20;
	}
}


void EU3Province::determineGoodsSupply(const tradeGoodMapping& tradeGoodMap)
{
	determinePopUnits();
	supply = popUnits;
	tradeGoodMapping::const_iterator goodItr = tradeGoodMap.find(tradeGood);
	for (vector< pair<string, double> >::const_iterator modifierItr = goodItr->second.supplyModifiers.begin(); modifierItr < goodItr->second.supplyModifiers.end(); modifierItr++)
	{
		if (modifierItr->first == "looted")
		{
			//if (looted) TODO
			//{
			//	supply *= modifierItr->second;
			//}
		}
		else if (modifierItr->first == "blockaded")
		{
			// can't happen
		}
		else if (modifierItr->first == "other controller")
		{
			//if (owner != controller) TODO
			//{
			//	supply *= modifierItr->second;
			//}
		}
		else if (modifierItr->first.substr(0, 14) == "stability not ")
		{
			int requiredStab = atoi( modifierItr->first.substr(14, 2).c_str() );
			if (owner->getStability() != requiredStab)
			{
				supply *= modifierItr->second;
			}
		}
		else if (modifierItr->first.substr(0, 27) == "aristocracy_plutocracy not ")
		{
		//	int requiredSlider = atoi( modifierItr->first.substr(27, 2).c_str() );
		//	if (country->getAristocracyPlutocracy() < requiredSlider) TODO
		//	{
		//		supply *= modifierItr->second;
		//	}
		}
		else if (modifierItr->first.substr(0, 15) == "land_naval not ")
		{
		//	int requiredSlider = atoi( modifierItr->first.substr(15, 2).c_str() );
		//	if (country->getLandNaval() < requiredSlider) TODO
		//	{
		//		supply *= modifierItr->second;
		//	}
		}
		else if (modifierItr->first.substr(0, 21) == "serfdom_freesubjects ")
		{
		//	int requiredSlider = atoi( modifierItr->first.substr(21, 2).c_str() );
		//	if (country->getSerfdomFreesubjects() >= requiredSlider) TODO
		//	{
		//		supply *= modifierItr->second;
		//	}
		}
		else if (modifierItr->first.substr(0, 23) == "mercantilism_freetrade ")
		{
		//	int requiredSlider = atoi( modifierItr->first.substr(23, 2).c_str() );
		//	if (country->getSerfdomFreesubjects() >= requiredSlider) TODO
		//	{
		//		supply *= modifierItr->second;
		//	}
		}
		else if (modifierItr->first.substr(0, 23) == "aristocracy_plutocracy ")
		{
		//	int requiredSlider = atoi( modifierItr->first.substr(23, 2).c_str() );
		//	if (country->getAristocracyPlutocracy() >= requiredSlider) TODO
		//	{
		//		supply *= modifierItr->second;
		//	}
		}
		else if (modifierItr->first.substr(0, 21) == "has country modifier ")
		{
			// no relevant modifiers at conversion
		}
		else if (modifierItr->first == "units_in_province")
		{
			if (armyHere)
			{
				supply *= modifierItr->second;
			}
		}
	}
}


void EU3Province::determineGoodsDemand(const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroupMap)
{
	for (tradeGoodMapping::const_iterator goodItr = tradeGoodMap.begin(); goodItr != tradeGoodMap.end(); goodItr++)
	{
		double demand = baseTax / 4000;
		vector< pair<vector<string>, double> > modifiers = goodItr->second.demandModifiers;
		for (vector< pair<vector<string>, double> >::iterator modItr = modifiers.begin(); modItr < modifiers.end(); modItr++)
		{
			vector<string> conditions = modItr->first;
			bool allConditions = true;
			for (vector<string>::iterator conditionItr = conditions.begin(); conditionItr < conditions.end(); conditionItr++)
			{
				if (*conditionItr == "war")
				{
					//if (!country->isAtWar()) TODO
					//{
						allConditions = false;
					//}
				} 
				else if (*conditionItr == "not war")
				{
					//if (country->isAtWar()) TODO
					//{
					//	allConditions = false;
					//}
				} 
				else if (conditionItr->substr(0, 13) == "stability is ")
				{
					int reqStability = atoi( conditionItr->substr(13, 2).c_str() );
					if (owner->getStability() < reqStability)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 14) == "government is ")
				{
					string reqGov = conditionItr->substr(14, 20);
					if (owner->getGovernment() != reqGov)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 14) == "land_naval is ")
				{
					//int reqSlider = atoi( conditionItr->substr(14, 2).c_str() ); TODO
					//if (country->getLandNaval() < reqSlider)
					//{
						allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 15) == "land_naval not ")
				{
					//int reqSlider = atoi( conditionItr->substr(15, 2).c_str() ); TODO
					//if (country->getLandNaval() >= reqSlider)
					//{
						allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 27) == "mercantilism_freetrade not ")
				{
					//int reqSlider = atoi( conditionItr->substr(27, 2).c_str() ); TODO
					//if (country->getMercantilismFreetrade() >= reqSlider)
					//{
					allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 21) == "quality_quantity not ")
				{
					//int reqSlider = atoi( conditionItr->substr(21, 2).c_str() ); TODO
					//if (country->getQualityQuantity() >= reqSlider)
					//{
						allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 25) == "serfdom_freesubjects not ")
				{
					//int reqSlider = atoi( conditionItr->substr(25, 2).c_str() ); TODO
					//if (country->getSerfdomFreesubjects() >= reqSlider)
					//{
						allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 14) == "has big_ships ")
				{
					//int reqShips = atoi( conditionItr->substr(14, 4).c_str() ); TODO
					//if (country->getBigShips() < reqShips)
					//{
						allConditions = false;
					//}
				}
				else if (conditionItr->substr(0, 14) == "has artillery ")
				{
						allConditions = false;
				}
				else if (conditionItr->substr(0, 13) == "has infantry ")
				{
					int reqUnits = atoi( conditionItr->substr(13, 4).c_str() );
					if (owner->getInfantry() < reqUnits)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 14) == "has land tech ")
				{
					int reqTech = atoi( conditionItr->substr(14, 4).c_str() );
					if (owner->getLandTech() < reqTech)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 15) == "tech group not ")
				{
					string reqTechGroup = conditionItr->substr(15, 15).c_str();
					if (owner->getTechGroup() == reqTechGroup)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 21) == "has country modifier ")
				{
					// no relevant modifiers at conversion
				}
				else if (conditionItr->substr(0, 9) == "prestige ")
				{
					double reqPrestige = atof( conditionItr->substr(9, 3).c_str() );
					if (owner->getPrestige() < reqPrestige)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 27) == "owner has this many ports: ")
				{
					double reqPorts = atof( conditionItr->substr(27, 4).c_str() );
					if (owner->getNumPorts() < reqPorts)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 28) == "trade income percentage not ")
				{
					// trade income always starts at 0 (no merchants)
				}
				else if (conditionItr->substr(0, 20) == "gold income percent ")
				{
					double reqPercent			= atof( conditionItr->substr(20, 4).c_str() );
					double tradeIncPercent	= owner->getGoldIncome() / owner->getIncome();
					if (tradeIncPercent < reqPercent)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 15) == "trade embargos ")
				{
					// no trade embargos at conversion
				}
				else if (conditionItr->substr(0, 16) == "trade goods not ")
				{
					string reqGood = conditionItr->substr(16, 10);
					if (tradeGood == reqGood)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 12) == "trade goods ")
				{
					string reqGood = conditionItr->substr(12, 10);
					if (tradeGood != reqGood)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 15) == "OR trade goods ")
				{
					string reqGood = conditionItr->substr(15, 10);
					if (tradeGood == reqGood)
					{
						break;
					}
				}
				else if (conditionItr->substr(0, 13) == "has building ")
				{
					string reqBuilding = conditionItr->substr(13, 10);
					bool hasBuilding = false;
					for (unsigned int i = 0; i < buildings.size(); i++)
					{
						if (buildings[i] == reqBuilding)
						{
							hasBuilding = true;
							break;
						}
					}
					if (!hasBuilding)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 17) == "has not building ")
				{
					string reqBuilding = conditionItr->substr(17, 10);
					bool hasBuilding = false;
					for (unsigned int i = 0; i < buildings.size(); i++)
					{
						if (buildings[i] == reqBuilding)
						{
							hasBuilding = true;
							break;
						}
					}
					if (hasBuilding)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 12) == "religion is ")
				{
					string reqReligion = conditionItr->substr(12, 10);
					if (religion != reqReligion)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 18) == "religion group is ")
				{
					string reqReligionGroup = conditionItr->substr(18, 10);
					if (EU3ReligionGroupMap.find(religion)->second != reqReligionGroup)
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 15) == "revolt risk is ")
				{
					//int reqRisk = atoi( conditionItr->substr(15, 2).c_str() );
					//if (revoltRisk < reqRisk)
					//{
						allConditions = false;
					//}
				}
				else if (*conditionItr == "overseas")
				{
					if (sameContinent || landConnection) // TODO: || distance <= 250
					{
						allConditions = false;
					}
				}
				else if (conditionItr->substr(0, 10) == "continent ")
				{
					string reqContinent = conditionItr->substr(10, 10);
					if (continent != reqContinent)
					{
						allConditions = false;
					}
				}
				else if (*conditionItr == "capital")
				{
					if (num != owner->getCapital())
					{
						allConditions = false;
					}
				}
				else if (*conditionItr == "looted")
				{
					//if (!looted) TODO
					//{
						allConditions = false;
					//}
				}
				else if (*conditionItr == "port")
				{
					if (!coastal)
					{
						allConditions = false;
					}
				}
			}
			if (allConditions)
			{
				demand *= modItr->second;
			}
		}
		demands.insert(make_pair(goodItr->first, demand));
	}
}


void EU3Province::addSupplyContribution(map<string, double>& goodsSupply)
{
	if (!tradeGood.empty())
	{
		goodsSupply[tradeGood] += supply / 100;
	}
}


void EU3Province::addDemandContribution(map<string, double>& goodsDemand)
{
	for (map<string, double>::const_iterator good = demands.begin(); good != demands.end(); good++)
	{
		goodsDemand[good->first] += good->second;
	}
}


double EU3Province::determineTax(const cultureGroupMapping& cultureGroups)
{
	double tax = baseTax;
	if (cot)
	{
		tax += 2;
		// tax += 2 * (COTSize / 100); TODO
	}
	for (unsigned int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i] == "workshop")
		{
			tax += 1;
		}
	}
	if (owner->getCapital() == num)
	{
		tax += 2;
	}

	tax /= 12;
	
	if (population <= 1000)
	{
		tax *= 0.10 * (population / 10);
	}
	if (owner->getPrimaryCulture() != culture)
	{
		bool acceptedCulture = false;
		vector<string> acceptedCultures = owner->getAcceptedCultures();
		for (unsigned int i = 0; i < acceptedCultures.size(); i++)
		{
			if (culture == acceptedCultures[i])
			{
				acceptedCulture = true;
				break;
			}
		}
		if (!acceptedCulture)
		{
			if ( cultureGroups.find(culture) == cultureGroups.find(owner->getPrimaryCulture()) )
			{
				tax *= 0.90;
			}
			else
			{
				tax *= 0.70;
			}
		}
	}
	if (religion != owner->getReligion())
	{
		tax *= 0.70;
	}
	//tax *= 1 - (int(revoltRisk / .01) * 0.05); TODO
	/*if (looted) TODO
	{
		tax *= 0.50;
	}*/
	/*if (scorched) TODO
	{
		tax *= 0.25;
	}*/
	if (!sameContinent && !landConnection) // TODO: && distance > 250
	{
		tax *= 0.10;
	}

	tax *= 1 + (0.10 * owner->getStability());
	//tax *= 1 + (country->getCentralizationDecentralization() * -0.04); TODO
	/*if (overseas)
	{
		tax *= 1 + (country->getLandNaval() * 0.05);
	}*/
	if (	(religion == "shiite") ||
			(religion == "bektashi") ||
			(religion == "druze") ||
			(religion == "hurufi") ||
			(religion == "shinto") 
		)
	{
		tax *= 0.80;
	}
	if (religion == "hinduism")
	{
		tax *= 1.05;
	}

	return tax;
}


double EU3Province::determineManu()
{
	for(unsigned int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i] == "university")
		{
			return 0.5;
		}
		else if (buildings[i] == "textile")
		{
			if ((tradeGood == "wool") || (tradeGood == "cloth"))
			{
				return 1.0;
			}
			else
			{
				return 0.5;
			}
		}
	}

	return 0.0;
}


double EU3Province::determineTolls()
{
	if (!sameContinent && !landConnection) // TODO: && distance > 250
	{
		return 0.0f;
	}

	double tolls;
	if (population <= 1000)
	{
		tolls = (10 * popUnits) / 12;
	}
	else
	{
		tolls = (10 + popUnits) / 12;
	}

	tolls *= owner->getTradeEffeciency();

	return tolls;
}


double EU3Province::determineProduction(const map<string, double>& unitPrices)
{
	double production = 0.0f;

	map<string, double>::const_iterator good = unitPrices.find(tradeGood);
	if (good != unitPrices.end())
	{
		production = popUnits * good->second;
		//if (tropical) TODO
		//{
		//	production *= 0.5;
		//}
		double prestige = owner->getPrestige();
		if (prestige > 0.15)
		{
			prestige = 0.15;
		}
		else if (prestige < -0.15)
		{
			prestige = -0.15;
		}
		production *= 1.0 + prestige;
		production *= owner->getProductionEffeciency();
		production /= 12;
	}

	return production;
}


double EU3Province::determineGold()
{
	if (tradeGood == "gold")
	{
		return (10.0 / 3.0) * popUnits;
	}
	else
	{
		return 0.0f;
	}
}


void EU3Province::setDiscoverers(map< string, vector<string> >& mapSpreadStrings)
{
	if (srcProvinceNums.size() > 0)
	{
		vector<string> discoverers = mapSpreadStrings["converted"];
		for (unsigned int j = 0; j < discoverers.size(); j++)
		{
			discoveredBy.push_back(discoverers[j]);
		}
	}
	for (unsigned int i = 0; i < rawDiscoverers.size(); i++)
	{
		vector<string> discoverers = mapSpreadStrings[ rawDiscoverers[i] ];
		for (unsigned int j = 0; j < discoverers.size(); j++)
		{
			discoveredBy.push_back(discoverers[j]);
		}
	}
}


void EU3Province::removeCore(EU3Country* country)
{
	for (vector<EU3Country*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
	{
		if (*coreItr == country)
		{
			cores.erase(coreItr);
			break;
		}
	}
}