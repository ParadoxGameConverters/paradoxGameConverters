/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "Mapper.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers\Object.h"
#include "EU4World\EU4World.h"
#include "EU4World\EU4Country.h"
#include "EU4World\EU4Province.h"
#include "V2World\V2World.h"
#include "V2World\V2Country.h"
#include <algorithm>
#include <sys/stat.h>


void initProvinceMap(Object* obj, const EU4Version* version, provinceMapping& provinceMap, provinceMapping& inverseProvinceMap, resettableMap& resettableProvinces)
{
	vector<Object*> versionLeaves = obj->getLeaves();	// the different version number blocks

	if (versionLeaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}
	
	unsigned int mappingIdx;	// the index to the version block we'll want to use
	for (mappingIdx = 0; mappingIdx < versionLeaves.size(); mappingIdx++)
	{
		if ((*version) >= EU4Version(versionLeaves[mappingIdx]->getKey()))
		{
			break;
		}
	}

	LOG(LogLevel::Debug) << "Using version " << versionLeaves[mappingIdx]->getKey() << " mappings";

	vector<Object*> data = versionLeaves[mappingIdx]->getLeaves();	// the actual mappings
	for (vector<Object*>::iterator i = data.begin(); i != data.end(); i++)
	{
		vector<int> EU4nums;					// the EU4 province numbers in this mappping
		vector<int> V2nums;					// the V2 province numbers in this mappping
		bool			resettable = false;	// if this is a province that can be reset to V2 defaults

		vector<Object*> euMaps = (*i)->getLeaves();	// the items within the mapping
		for (vector<Object*>::iterator j = euMaps.begin(); j != euMaps.end(); j++)
		{
			if ( (*j)->getKey() == "eu4" )
			{
				EU4nums.push_back(  atoi( (*j)->getLeaf().c_str() )  );
			}
			else if ( (*j)->getKey() == "v2" )
			{
				V2nums.push_back(  atoi( (*j)->getLeaf().c_str() )  );
			}
			else if ( (*j)->getKey() == "resettable" )
			{
				resettable = true;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
			}
		}

		if (EU4nums.size() == 0)
		{
			EU4nums.push_back(0);
		}
		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}

		for (vector<int>::iterator j = V2nums.begin(); j != V2nums.end(); j++)
		{
			if (*j != 0)
			{
				provinceMap.insert(make_pair(*j, EU4nums));
				if (resettable)
				{
					resettableProvinces.insert(*j);
				}
			}
		}
		for (vector<int>::iterator j = EU4nums.begin(); j != EU4nums.end(); j++)
		{
			if (*j != 0)
			{
				inverseProvinceMap.insert(make_pair(*j, V2nums));
			}
		}
	}
}


static const vector<int> empty_vec;	// an empty vector in case there are no equivalent V2 province numbers
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, const int eu4ProvinceNum)
{
	inverseProvinceMapping::const_iterator itr = invProvMap.find(eu4ProvinceNum);	// the province entry in the inverse province map
	if (itr == invProvMap.end())
	{
		return empty_vec;
	}
	else
	{
		return itr->second;
	}
}

typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
	int pathX;			// the midpoint on the path drawn between provinces
	int pathY;			// the midpoint on the path drawn between provinces
	int unknown3;		// still unknown
	int unknown4;		// still unknown
} HODAdjacency;		// an entry in the HOD adjacencies.bin format
typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
	int pathX;			// the midpoint on the path drawn between provinces
	int pathY;			// the midpoint on the path drawn between provinces
} AHDAdjacency;		// an entry in the AHD adjacencies.bin format
typedef struct {
	int type;			// the type of adjacency 0 = normal, 1 = ford, 2 = river crossing
	int to;				// the province this one is adjacent to (expect one pointing back to this province)
	int via;				// the straight (if any) this crosses
	int unknown1;		// still unknown
	int unknown2;		// still unknown
} VanillaAdjacency;	// an entry in the vanilla adjacencies.bin format
adjacencyMapping initAdjacencyMap()
{
	FILE* adjacenciesBin = NULL;	// the adjacencies.bin file
	string filename = Configuration::getV2DocumentsPath() + "\\map\\cache\\adjacencies.bin";	// the path and filename for adjacencies.bin
	struct _stat st;	// the data structure telling us if the file exists
	if ((_stat(filename.c_str(), &st) != 0))
	{
		LOG(LogLevel::Warning) << "Could not find " << filename << " - looking in install folder";
		filename = Configuration::getV2Path() + "\\map\\cache\\adjacencies.bin";
	}
	fopen_s(&adjacenciesBin, filename.c_str(), "rb");
	if (adjacenciesBin == NULL)
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(1);
	}

	adjacencyMapping adjacencyMap;	// the adjacency mapping
	while (!feof(adjacenciesBin))
	{
		int numAdjacencies;	// the total number of adjacencies
		if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesBin) != 1)
		{
			break;
		}
		vector<int> adjacencies;	// the adjacencies for the current province
		for (int i = 0; i < numAdjacencies; i++)
		{
			if (Configuration::getV2Gametype() == "vanilla")
			{
				VanillaAdjacency readAdjacency;
				fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesBin);
				adjacencies.push_back(readAdjacency.to);
			}
			else if (Configuration::getV2Gametype() == "AHD")
			{
				AHDAdjacency readAdjacency;
				fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesBin);
				adjacencies.push_back(readAdjacency.to);
			}
			if ((Configuration::getV2Gametype() == "HOD") || (Configuration::getV2Gametype() == "HoD-NNM"))
			{
				HODAdjacency readAdjacency;
				fread(&readAdjacency, sizeof(readAdjacency), 1, adjacenciesBin);
				adjacencies.push_back(readAdjacency.to);
			}
		}
		adjacencyMap.push_back(adjacencies);
	}
	fclose(adjacenciesBin);

	// optional code to output data from the adjacencies map
	FILE* adjacenciesData;
	fopen_s(&adjacenciesData, "adjacenciesData.csv", "w");
	fprintf(adjacenciesData, "From,To\n");
	for (unsigned int from = 0; from < adjacencyMap.size(); from++)
	{
		vector<int> adjacencies = adjacencyMap[from];
		for (unsigned int i = 0; i < adjacencies.size(); i++)
		{
			fprintf(adjacenciesData, "%d,%d\n", from, adjacencies[i]);
		}
	}
	fclose(adjacenciesData);

	return adjacencyMap;
}


void initContinentMap(Object* obj, continentMapping& continentMap)
{
	continentMap.clear();
	vector<Object*> continentObjs = obj->getLeaves();	// the continents
	for (unsigned int i = 0; i < continentObjs.size(); i++)
	{
		string continent = continentObjs[i]->getKey();	// the current continent
		vector<string> provinceNums = continentObjs[i]->getTokens();	// the province numbers in this continent
		for (unsigned int j = 0; j < provinceNums.size(); j++)
		{
			const int province = atoi(provinceNums[j].c_str());	// the current province
			continentMap.insert( make_pair(province, continent) );
		}
	}
}


void mergeNations(EU4World& world, Object* mergeObj)
{
	vector<Object*> rules = mergeObj->getValue("merge_nations");	// all merging rules
	if (rules.size() < 0)
	{
		LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
		return;
	}

	rules = rules[0]->getLeaves();	// the rules themselves
	for (vector<Object*>::iterator itr = rules.begin(); itr != rules.end(); ++itr)
	{
		if ((*itr)->getKey() == "merge_daimyos")
		{
			if ((*itr)->getLeaf() == "yes")
			{
				uniteJapan(world);
			}
			continue;
		}

		vector<Object*> thisMerge = (*itr)->getLeaves();	// the current merge rule
		string masterTag;												// the nation to merge into
		vector<string> slaveTags;									// the nations that will be merged into the master
		bool enabled = false;										// whether or not this rule is enabled
		for (vector<Object*>::iterator jtr = thisMerge.begin(); jtr != thisMerge.end(); ++jtr)
		{
			if ((*jtr)->getKey() == "merge" && (*jtr)->getLeaf() == "yes")
			{
				enabled = true;
			}
			else if ((*jtr)->getKey() == "master")
			{
				masterTag = (*jtr)->getLeaf();
			}
			else if ((*jtr)->getKey() == "slave")
			{
				slaveTags.push_back((*jtr)->getLeaf());
			}
		}
		EU4Country* master = world.getCountry(masterTag);	// the actual master country
		if ( enabled && (master != NULL) && (slaveTags.size() > 0) )
		{
			for (vector<string>::iterator sitr = slaveTags.begin(); sitr != slaveTags.end(); ++sitr)
			{
				master->eatCountry(world.getCountry(*sitr));
			}
		}
	}
}


void uniteJapan(EU4World& world)
{
	EU4Country* japan = world.getCountry("JAP");	// a pointer to EU4 Japan
	if (japan == NULL)
	{
		return;
	}
	if( japan->hasFlag("united_daimyos_of_japan") )
	{
		return;
	}

	map<string, EU4Country*> countries = world.getCountries();	// the countries in the world
	for (map<string, EU4Country*>::iterator i = countries.begin(); i != countries.end(); ++i)
	{
		if ( i->second->getPossibleDaimyo() )
		{
			japan->eatCountry(i->second);
		}
	}
}


void removeEmptyNations(EU4World& world)
{
	map<string, EU4Country*> countries = world.getCountries();	// all EU4 countries
	for (map<string, EU4Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		vector<EU4Province*> provinces	= i->second->getProvinces();	// the provinces for the nation
		vector<EU4Province*> cores			= i->second->getCores();		// the cores for the nation
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			world.removeCountry(i->first);
			LOG(LogLevel::Debug) << "Removing empty nation " << i->first;
		}
	}
}


void removeDeadLandlessNations(EU4World& world)
{
	map<string, EU4Country*> allCountries = world.getCountries();	// all the EU4 countries

	map<string, EU4Country*> landlessCountries;	// all the landless EU4 countries
	for (map<string, EU4Country*>::iterator i = allCountries.begin(); i != allCountries.end(); i++)
	{
		vector<EU4Province*> provinces = i->second->getProvinces();	// the provinces for this country
		if (provinces.size() == 0)
		{
			landlessCountries.insert(*i);
		}
	}

	for (map<string, EU4Country*>::iterator countryItr = landlessCountries.begin(); countryItr != landlessCountries.end(); countryItr++)
	{
		string primaryCulture		= countryItr->second->getPrimaryCulture();	// the primary culture of this country
		vector<EU4Province*> cores	= countryItr->second->getCores();				// the cores of this country
		bool cultureSurvives			= false;													// whether or not the primary culture survives in any of the cores
		for (vector<EU4Province*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
		{
			if ( (*coreItr)->getOwner() == NULL)
			{
				continue;
			}
			if (  (*coreItr)->getOwner()->getPrimaryCulture() == primaryCulture  )
			{
				continue;
			}

			vector<EU4PopRatio> popRatios = (*coreItr)->getPopRatios();	// the population ratios for this province
			double culturePercent = 0.0f;											// the percentage of the primary culture in this province
			for (vector<EU4PopRatio>::iterator popItr = popRatios.begin(); popItr != popRatios.end(); popItr++)
			{
				if (popItr->culture == primaryCulture)
				{
					culturePercent += popItr->popRatio;
				}
			}
			if ( culturePercent >= 0.5 )
			{
				cultureSurvives = true;
				break;
			}
		}

		if (cultureSurvives == false)
		{
			world.removeCountry( countryItr->first );
			LOG(LogLevel::Debug) << "Removing dead landless nation " << countryItr->first;
		}
	}
}


void removeLandlessNations(EU4World& world)
{
	map<string, EU4Country*> countries = world.getCountries();	// all EU4 countries

	for (map<string, EU4Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		vector<EU4Province*> provinces = i->second->getProvinces();	// the provinces for this country
		if (provinces.size() == 0)
		{
			world.removeCountry(i->first);
			LOG(LogLevel::Debug) << "Removing landless nation " << i->first;
		}
	}
}


void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap)
{
	vector<Object*> leaves = obj->getLeaves();	// the states

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		vector<string> provinces = leaves[i]->getTokens();	// the provinces in this state
		vector<int>		neighbors;									// the neighboring provinces (that is, all provinces in the state)

		for (vector<string>::iterator j = provinces.begin(); j != provinces.end(); j++)
		{
			neighbors.push_back( atoi(j->c_str()) );
			stateIndexMap.insert(make_pair(atoi(j->c_str()), i));
		}
		for (vector<int>::iterator j = neighbors.begin(); j != neighbors.end(); j++)
		{
			stateMap.insert(make_pair(*j, neighbors));
		}
	}
}


cultureMapping initCultureMap(Object* obj)
{
	cultureMapping cultureMap;						// the culture mapping
	vector<Object*> links = obj->getLeaves();	// the culture mapping rules

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>			cultures	= (*i)->getLeaves();	// the items in this rule

		vector<string>				srcCultures;		// the EU4 cultures
		string						dstCulture;			// the V2 culture
		vector< distinguisher > distinguishers;	// any rules for distinguishing V2 cultures from the same EU4 cultures
		for (vector<Object*>::iterator j = cultures.begin(); j != cultures.end(); j++)
		{
			if ( (*j)->getKey() == "v2" )
			{
				dstCulture = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu4" )
			{
				srcCultures.push_back( (*j)->getLeaf() );
			}
			if ( (*j)->getKey() == "owner" )
			{
				distinguisher newD;	// a new distinguiser
				newD.first	= DTOwner;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "religion" )
			{
				distinguisher newD;	// a new distinguiser
				newD.first	= DTReligion;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
		}

		for (vector<string>::iterator j = srcCultures.begin(); j != srcCultures.end(); j++)
		{
			cultureStruct rule;	// the new culture rule
			rule.srcCulture		= (*j);
			rule.dstCulture		= dstCulture;
			rule.distinguishers	= distinguishers;
			cultureMap.push_back(rule);
		}
	}

	return cultureMap;
}


religionMapping initReligionMap(Object* obj)
{
	religionMapping religionMap;					// the religion mapping
	vector<Object*> links = obj->getLeaves();	// the religion mapping rules

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	religions	= (*i)->getLeaves();	// the items in this rule
		string				dstReligion;							// the V2 religion
		vector<string>		srcReligion;							// the EU4 religions

		for (vector<Object*>::iterator j = religions.begin(); j != religions.end(); j++)
		{
			if ( (*j)->getKey() == "v2" )
			{
				dstReligion = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu4" )
			{
				srcReligion.push_back( (*j)->getLeaf() );
			}
		}

		for (vector<string>::iterator j = srcReligion.begin(); j != srcReligion.end(); j++)
		{
			religionMap.insert(make_pair((*j), dstReligion));
		}
	}

	return religionMap;
}


unionMapping initUnionMap(Object* obj)
{
	unionMapping unionMap;	// the cultural unions map

	vector<Object*> unions = obj->getLeaves();	// the rules for cultural unions
	for (vector<Object*>::iterator i = unions.begin(); i != unions.end(); i++)
	{
		string tag;			// the tag for the cultural union
		string culture;	// the culture for the cultural union

		vector<Object*> aUnion = (*i)->getLeaves();	// the items for this rule
		for (vector<Object*>::iterator j = aUnion.begin(); j != aUnion.end(); j++)
		{
			if ( (*j)->getKey() == "tag" )
			{
				tag = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "culture" )
			{
				culture = (*j)->getLeaf();
			}
		}

		unionMap.push_back(make_pair(culture, tag));
	}

	return unionMap;
}


governmentMapping initGovernmentMap(Object* obj)
{
	governmentMapping governmentMap;				// the government mapping

	vector<Object*> links = obj->getLeaves();	// rules for the government mapping
	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	governments	= (*i)->getLeaves();	// the items for this rule
		string				dstGovernment;							// the V2 government
		vector<string>		srcGovernments;						// the EU4 governments

		for (vector<Object*>::iterator j = governments.begin(); j != governments.end(); j++)
		{
			if ( (*j)->getKey() == "v2" )
			{
				dstGovernment = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu4" )
			{
				srcGovernments.push_back( (*j)->getLeaf() );
			}
		}

		for (vector<string>::iterator j = srcGovernments.begin(); j != srcGovernments.end(); j++)
		{
			governmentMap.insert(make_pair((*j), dstGovernment));
		}
	}

	return governmentMap;
}


void initUnionCultures(Object* obj, unionCulturesMap& unionCultures, inverseUnionCulturesMap& inverseUnionCultures)
{
	vector<Object*> cultureGroups = obj->getLeaves();	// the cultural group rules
	for (vector<Object*>::iterator i = cultureGroups.begin(); i != cultureGroups.end(); i++)
	{
		vector<Object*>		culturesObj		= (*i)->getLeaves();	// the items in this rule
		string					group				= (*i)->getKey();		// the cultural group
		vector<string>			cultures;									// the cultures

		for (vector<Object*>::iterator j = culturesObj.begin(); j != culturesObj.end(); j++)
		{
			if ( (*j)->getKey() == "dynasty_names" )
			{
				continue;
			}
			else if ((*j)->getKey() == "graphical_culture")
			{
				continue;
			}
			else
			{
				cultures.push_back( (*j)->getKey() );
				inverseUnionCultures.insert(make_pair((*j)->getKey(), group));
			}
		}

		unionCulturesMap::iterator itr = unionCultures.find(group);
		if (itr != unionCultures.end())
		{
			vector<string> oldCultures = itr->second;	// any cultures already in the group
			for (vector<string>::iterator jtr = oldCultures.begin(); jtr != oldCultures.end(); jtr++)
			{
				cultures.push_back(*jtr);
			}
		}
		unionCultures[group] = cultures;
	}
}


void initIdeaEffects(Object* obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas)
{
	vector<Object*> ideasObj = obj->getLeaves();
	for (vector<Object*>::iterator ideasItr = ideasObj.begin(); ideasItr != ideasObj.end(); ideasItr++)
	{
		string idea = (*ideasItr)->getKey();
		vector<Object*> effects = (*ideasItr)->getLeaves();
		for (vector<Object*>::iterator effectsItr = effects.begin(); effectsItr != effects.end(); effectsItr++)
		{
			string effectType = (*effectsItr)->getKey();
			if (effectType == "army_investment")
			{
				armyInvIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "commerce_investment")
			{
				commerceInvIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "culture_investment")
			{
				cultureInvIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "industry_investment")
			{
				industryInvIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "navy_investment")
			{
				navyInvIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "upper_house_liberal")
			{
				UHLiberalIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "upper_house_reactionary")
			{
				UHReactionaryIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "NV_order")
			{
				orderIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "NV_liberty")
			{
				libertyIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "NV_equality")
			{
				equalityIdeas[idea] = atoi((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "literacy")
			{
				vector<string> literacyStrs = (*effectsItr)[0].getTokens();
				for (unsigned int i = 0; i < literacyStrs.size(); i++)
				{
					literacyIdeas.push_back(make_pair(idea, atoi(literacyStrs[i].c_str())));
				}
			}
		}
	}
}


colonyMapping initColonyMap(Object* obj)
{
	colonyMapping colonyMap;													// the culture mapping
	vector<Object*> colonialRules	= obj->getLeaves();					// the culture mapping rules en masse
	vector<Object*> links			= colonialRules[0]->getLeaves();	// the individual culture mapping rules

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>			items = (*i)->getLeaves();	// the items in this rule

		colonyStruct rule;	// the new culture rule
		for (vector<Object*>::iterator j = items.begin(); j != items.end(); j++)
		{
			if ((*j)->getKey() == "tag")
			{
				rule.tag = (*j)->getLeaf();
			}
			if ((*j)->getKey() == "EU4_region")
			{
				rule.EU4Region = (*j)->getLeaf();
			}
			if ((*j)->getKey() == "V2_region")
			{
				rule.V2Region = (*j)->getLeaf();
			}
			if ((*j)->getKey() == "is_culture_group")
			{
				rule.cultureGroup = (*j)->getLeaf();
			}
		}

		colonyMap.push_back(rule);
	}

	return colonyMap;
}

colonyFlagset initColonyFlagset(Object* obj)
{
	colonyFlagset colonyMap;									// the culture mapping
	vector<Object*> colonialRules = obj->getLeaves();			// the culture mapping rules en masse
	vector<Object*> regions = colonialRules[1]->getLeaves();	// the individual colonial flags

	for (vector<Object*>::iterator i = regions.begin(); i != regions.end(); i++)
	{
		std::string region = (*i)->getKey();
		vector<Object*> flags = (*i)->getLeaves();	// the flags in this region
		for (vector<Object*>::iterator j = flags.begin(); j != flags.end(); j++)
		{
			vector<Object*>			items = (*j)->getLeaves();	// the items in this rule
			
			shared_ptr<colonyFlag> flag(new colonyFlag());
			flag->region = region;
			flag->unique = false;
			flag->overlord = "";
			vector<Object*>::iterator k = items.begin();
			flag->name = (*k)->getLeaf();
			std::transform(flag->name.begin(), flag->name.end(), flag->name.begin(), ::tolower);

			for (; k != items.end(); k++)
			{
				if ((*k)->getKey() == "name")
				{
					string name = (*k)->getLeaf();
					name = V2Localisation::Convert(name);
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);

					colonyMap[name] = flag;
				}
				if ((*k)->getKey() == "unique")
				{
					flag->unique = true;
				}

			}
		}
	}

	return colonyMap;
}

ck2TitleMapping initCK2TitleMap(Object* obj)
{
	ck2TitleMapping titleMap;
	vector<Object*> titles = obj->getLeaves();
	vector<Object*> links = titles[0]->getLeaves();	

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	titles	= (*i)->getLeaves();
		string name;
		string titleID;

		for (vector<Object*>::iterator j = titles.begin(); j != titles.end(); j++)
		{
			if ( (*j)->getKey() == "name" )
			{
				name = (*j)->getLeaf();
				name = V2Localisation::Convert(name);
			}
			if ( (*j)->getKey() == "title" )
			{
				titleID = (*j)->getLeaf();
			}
		}

		titleMap[name] = titleID;
	}

	return titleMap;
}


string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}