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
#include "EU3World/EU3World.h"
#include "EU3World/EU3Country.h"
#include "EU3World/EU3Province.h"
#include "V2World/V2World.h"
#include "V2World/V2Country.h"
#include <algorithm>
#include <sys/stat.h>



void initProvinceMap(Object* obj, WorldType worldType, provinceMapping& provinceMap, inverseProvinceMapping& inverseProvinceMap, resettableMap& resettableProvinces)
{
	provinceMapping::iterator mapIter;

	vector<Object*> worldTypeLeaves = obj->getLeaves();

	if (worldTypeLeaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}

	unsigned int mappingIdx = -1;
	for (unsigned int i = 0; i < worldTypeLeaves.size(); i++)
	{
		switch(worldType)
		{
			case InNomine:
			{
				if (worldTypeLeaves[i]->getKey() == "in")
				{
					mappingIdx = i;
				}
				break;
			}
			case HeirToTheThrone:
			{
				if (worldTypeLeaves[i]->getKey() == "httt")
				{
					mappingIdx = i;
				}
				break;
			}
			case DivineWind:
			{
				if (worldTypeLeaves[i]->getKey() == "dw")
				{
					mappingIdx = i;
				}
				break;
			}
			case unknown:
			case VeryOld:
			default:
			{
				Log(LogLevel::Error) << "Unsupported world type. Cannot map provinces!";
				exit(-1);
			}
		}
	}

	LOG(LogLevel::Debug) << "Using world type " << worldTypeLeaves[mappingIdx]->getKey() << " mappings";
	vector<Object*> data = worldTypeLeaves[mappingIdx]->getLeaves();

	for (vector<Object*>::iterator i = data.begin(); i != data.end(); i++)
	{
		vector<int> EU3nums;
		vector<int> V2nums;
		bool			resettable = false;

		vector<Object*> euMaps = (*i)->getLeaves();

		for (vector<Object*>::iterator j = euMaps.begin(); j != euMaps.end(); j++)
		{
			if ( (*j)->getKey() == "eu3" )
			{
				EU3nums.push_back(  atoi( (*j)->getLeaf().c_str() )  );
			}
			else if ( (*j)->getKey() == "vic" )
			{
				V2nums.push_back(  atoi( (*j)->getLeaf().c_str() )  );
			}
			else if ((*j)->getKey() == "resettable")
			{
				resettable = true;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
			}
		}

		if (EU3nums.size() == 0)
		{
			EU3nums.push_back(0);
		}
		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}

		for (vector<int>::iterator j = V2nums.begin(); j != V2nums.end(); j++)
		{
			if (*j != 0)
			{
				provinceMap.insert(make_pair(*j, EU3nums));
			}
			if (resettable)
			{
				resettableProvinces.insert(*j);
			}
		}
		for (vector<int>::iterator j = EU3nums.begin(); j != EU3nums.end(); j++)
		{
			if (*j != 0)
			{
				inverseProvinceMap.insert(make_pair(*j, V2nums));
			}
		}
	}
}


static const vector<int> empty_vec;
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum)
{
	inverseProvinceMapping::const_iterator itr = invProvMap.find(eu3ProvinceNum);
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
	FILE* adjacenciesBin = NULL;
	string filename = Configuration::getV2DocumentsPath() + "\\map\\cache\\adjacencies.bin";
	struct _stat st;
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

	adjacencyMapping adjacencyMap;
	while (!feof(adjacenciesBin))
	{
		int numAdjacencies;
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

	/*FILE* adjacenciesData;
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
	fclose(adjacenciesData);*/
	
	return adjacencyMap;
}


void initContinentMap(Object* obj, continentMapping& continentMap)
{
	continentMap.clear();
	vector<Object*> continentObjs = obj->getLeaves();
	for (unsigned int i = 0; i < continentObjs.size(); i++)
	{
		string continent = continentObjs[i]->getKey();
		vector<string> provinceNums = continentObjs[i]->getTokens();
		for (unsigned int j = 0; j < provinceNums.size(); j++)
		{
			int province = atoi(provinceNums[j].c_str());
			continentMap.insert(make_pair(province, continent));
		}
	}
}


void mergeNations(EU3World& world, Object* mergeObj)
{
	vector<Object*> rules = mergeObj->getValue("merge_nations");
	if (rules.size() < 0)
	{
		LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
		return;
	}

	rules = rules[0]->getLeaves();
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

		vector<Object*> thisMerge = (*itr)->getLeaves();
		string masterTag;
		vector<string> slaveTags;
		bool enabled = false;
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
		EU3Country* master = world.getCountry(masterTag);
		if ( enabled && (master != NULL) && (slaveTags.size() > 0) )
		{
			for (vector<string>::iterator sitr = slaveTags.begin(); sitr != slaveTags.end(); ++sitr)
			{
				master->eatCountry(world.getCountry(*sitr));
			}
		}
	}
}


void uniteJapan(EU3World& world)
{
	EU3Country* japan = world.getCountry("JAP");
	if (japan == NULL)
	{
		return;
	}
	if( japan->hasFlag("united_daimyos_of_japan") )
	{
		return;
	}

	map<string, EU3Country*> countries = world.getCountries();
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); ++i)
	{
		if ( i->second->getPossibleDaimyo() )
		{
			japan->eatCountry(i->second);
		}
	}
}


void removeEmptyNations(EU3World& world)
{
	map<string, EU3Country*> countries = world.getCountries();
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		vector<EU3Province*> provinces	= i->second->getProvinces();
		vector<EU3Province*> cores			= i->second->getCores();
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			world.removeCountry(i->first);
			LOG(LogLevel::Debug) << "Removing empty nation " << i->first;
		}
	}
}


void removeDeadLandlessNations(EU3World& world)
{
	map<string, EU3Country*> allCountries = world.getCountries();

	map<string, EU3Country*> landlessCountries;
	for (map<string, EU3Country*>::iterator i = allCountries.begin(); i != allCountries.end(); i++)
	{
		vector<EU3Province*> provinces = i->second->getProvinces();
		if (provinces.size() == 0)
		{
			landlessCountries.insert(*i);
		}
	}

	for (map<string, EU3Country*>::iterator countryItr = landlessCountries.begin(); countryItr != landlessCountries.end(); countryItr++)
	{
		string primaryCulture		= countryItr->second->getPrimaryCulture();
		vector<EU3Province*> cores	= countryItr->second->getCores();
		bool cultureSurvives			= false;
		for (vector<EU3Province*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
		{
			if ( (*coreItr)->getOwner() == NULL)
			{
				continue;
			}
			if (  (*coreItr)->getOwner()->getPrimaryCulture() == primaryCulture  )
			{
				continue;
			}

			vector<EU3PopRatio> popRatios = (*coreItr)->getPopRatios();
			double culturePercent = 0.0f;
			for (vector<EU3PopRatio>::iterator popItr = popRatios.begin(); popItr != popRatios.end(); popItr++)
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


void removeLandlessNations(EU3World& world)
{
	map<string, EU3Country*> countries = world.getCountries();

	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		vector<EU3Province*> provinces = i->second->getProvinces();
		if (provinces.size() == 0)
		{
			world.removeCountry(i->first);
			LOG(LogLevel::Debug) << "Removing landless nation " << i->first;
		}
	}
}


void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap)
{
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		vector<string> provinces = leaves[i]->getTokens();
		vector<int>		neighbors;

		for (vector<string>::iterator j = provinces.begin(); j != provinces.end(); j++)
		{
			neighbors.push_back( atoi(j->c_str()) );
			stateIndexMap.insert( make_pair(atoi(j->c_str()), i) );
		}
		for (vector<int>::iterator j = neighbors.begin(); j != neighbors.end(); j++)
		{
			stateMap.insert(make_pair(*j, neighbors));
		}
	}
}


cultureMapping initCultureMap(Object* obj) // TODO: consider cleaning up the distinguishers
{
	cultureMapping cultureMap;
	vector<Object*> links = obj->getLeaves();

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>			cultures	= (*i)->getLeaves();

		vector<string>				srcCultures;
		string						dstCulture;
		vector< distinguisher > distinguishers;
		for (vector<Object*>::iterator j = cultures.begin(); j != cultures.end(); j++)
		{
			if ( (*j)->getKey() == "vic" )
			{
				dstCulture = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu3" )
			{
				srcCultures.push_back( (*j)->getLeaf() );
			}
			if ( (*j)->getKey() == "owner" )
			{
				distinguisher newD;
				newD.first	= DTOwner;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "religion" )
			{
				distinguisher newD;
				newD.first	= DTReligion;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "region" )
			{
				distinguisher newD;	// a new distinguiser
				newD.first	= DTRegion;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
		}

		for (vector<string>::iterator j = srcCultures.begin(); j != srcCultures.end(); j++)
		{
			cultureStruct rule;
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
	religionMapping religionMap;
	vector<Object*> links = obj->getLeaves();

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	religions	= (*i)->getLeaves();
		string				dstReligion;
		vector<string>		srcReligion;

		for (vector<Object*>::iterator j = religions.begin(); j != religions.end(); j++)
		{
			if ( (*j)->getKey() == "vic" )
			{
				dstReligion = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu3" )
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
	unionMapping unionMap;

	vector<Object*> unions = obj->getLeaves();
	for (vector<Object*>::iterator i = unions.begin(); i != unions.end(); i++)
	{
		string tag;
		string culture;

		vector<Object*> aUnion = (*i)->getLeaves();
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
	governmentMapping governmentMap;
	vector<Object*> links = obj->getLeaves();

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	governments	= (*i)->getLeaves();
		string				dstGovernment;
		vector<string>		srcGovernments;

		for (vector<Object*>::iterator j = governments.begin(); j != governments.end(); j++)
		{
			if ( (*j)->getKey() == "vic" )
			{
				dstGovernment = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "eu3" )
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
		bool						hasUnion			= false;
		string					tag;
		vector<string>			cultures;

		for (vector<Object*>::iterator j = culturesObj.begin(); j != culturesObj.end(); j++)
		{
			if ( (*j)->getKey() == "union")
			{
				hasUnion	= true;
				tag		= (*j)->getLeaf();
			}
			else if ( (*j)->getKey() == "dynasty_names" )
			{
				continue;
			}
			else
			{
				cultures.push_back( (*j)->getKey() );
				inverseUnionCultures.insert(make_pair((*j)->getKey(), group));
			}
		}

		if (hasUnion)
		{
			unionCultures[tag] = cultures;
		}
	}
}


void initEU3RegionMap(Object *obj, EU3RegionsMapping& regions)
{
	regions.clear();
	vector<Object*> regionsObj = obj->getLeaves();	// the regions themselves
	for (vector<Object*>::iterator regionsItr = regionsObj.begin(); regionsItr != regionsObj.end(); regionsItr++)
	{
		string regionName = (*regionsItr)->getKey();
		vector<string> provinceStrings = (*regionsItr)->getTokens();				// the province numbers
		for (vector<string>::iterator provinceItr = provinceStrings.begin(); provinceItr != provinceStrings.end(); provinceItr++)
		{
			int provinceNum = atoi(provinceItr->c_str());
			auto mapping = regions.find(provinceNum);
			if (mapping == regions.end())
			{
				set<string> newRegions;
				newRegions.insert(regionName);
				regions.insert(make_pair(provinceNum, newRegions));
			}
			else
			{
				mapping->second.insert(regionName);
			}
		}
	}
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