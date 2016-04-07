/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "Object.h"
#include "V2World\V2World.h"
#include "V2World\V2Country.h"
#include "V2World\V2Province.h"
#include <algorithm>
#include <sys/stat.h>



void initProvinceMap(Object* obj, provinceMapping& provinceMap, provinceMapping& inverseProvinceMap, resettableMap& resettableProvinces)
{
	vector<Object*> leaves = obj->getLeaves();	// the different version number blocks

	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}

	vector<Object*> data = leaves[0]->getLeaves();	// the actual mappings
	for (vector<Object*>::iterator i = data.begin(); i != data.end(); i++)
	{
		vector<int> V2nums;					// the V2 province numbers in this mappping
		vector<int> HoI3nums;				// the HoI3 province numbers in this mappping
		bool			resettable = false;	// if this is a province that can be reset to V2 defaults

		vector<Object*> vMaps = (*i)->getLeaves();	// the items within the mapping
		for (vector<Object*>::iterator j = vMaps.begin(); j != vMaps.end(); j++)
		{
			if ((*j)->getKey() == "vic")
			{
				V2nums.push_back(atoi((*j)->getLeaf().c_str()));
			}
			else if ((*j)->getKey() == "hoi")
			{
				HoI3nums.push_back(atoi((*j)->getLeaf().c_str()));
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

		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}
		if (HoI3nums.size() == 0)
		{
			HoI3nums.push_back(0);
		}

		for (vector<int>::iterator j = HoI3nums.begin(); j != HoI3nums.end(); j++)
		{
			if (*j != 0)
			{
				provinceMap.insert(make_pair(*j, V2nums));
				if (resettable)
				{
					resettableProvinces.insert(*j);
				}
			}
		}
		for (vector<int>::iterator j = V2nums.begin(); j != V2nums.end(); j++)
		{
			if (*j != 0)
			{
				inverseProvinceMap.insert(make_pair(*j, HoI3nums));
			}
		}
	}
}


static const vector<int> empty_vec;	// an empty vector in case there are no equivalent V2 province numbers
vector<int> getHoI3ProvinceNums(inverseProvinceMapping invProvMap, const int v2ProvinceNum)
{
	inverseProvinceMapping::iterator itr = invProvMap.find(v2ProvinceNum);	// the province entry in the inverse province map
	if (itr == invProvMap.end())
	{
		return empty_vec;
	}
	else
	{
		return itr->second;
	}
}


HoI3AdjacencyMapping initHoI3AdjacencyMap()
{
	FILE* adjacenciesBin = NULL;	// the adjacencies.bin file
	string filename = Configuration::getHoI3Path() + "\\tfh\\map\\cache\\adjacencies.bin";
	fopen_s(&adjacenciesBin, filename.c_str(), "rb");
	if (adjacenciesBin == NULL)
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(1);
	}

	HoI3AdjacencyMapping adjacencyMap;	// the adjacency mapping
	while (!feof(adjacenciesBin))
	{
		int numAdjacencies;	// the total number of adjacencies
		if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesBin) != 1)
		{
			break;
		}
		vector<adjacency> adjacencies;	// the adjacencies for the current province
		for (int i = 0; i < numAdjacencies; i++)
		{
			adjacency newAdjacency;	// the current adjacency
			fread(&newAdjacency, sizeof(newAdjacency), 1, adjacenciesBin);
			adjacencies.push_back(newAdjacency);
		}
		adjacencyMap.push_back(adjacencies);
	}
	fclose(adjacenciesBin);

	// optional code to output data from the adjacencies map
	/*FILE* adjacenciesData;
	fopen_s(&adjacenciesData, "adjacenciesData.csv", "w");
	fprintf(adjacenciesData, "From,Type,To,Via,Unknown1,Unknown2\n");
	for (unsigned int from = 0; from < adjacencyMap.size(); from++)
	{
		vector<adjacency> adjacencies = adjacencyMap[from];
		for (unsigned int i = 0; i < adjacencies.size(); i++)
		{
			fprintf(adjacenciesData, "%d,%d,%d,%d,%d,%d\n", from, adjacencies[i].type, adjacencies[i].to, adjacencies[i].via, adjacencies[i].unknown1, adjacencies[i].unknown2);
		}
	}
	fclose(adjacenciesData);*/

	return adjacencyMap;
}


void initContinentMap(Object* obj, continentMapping& continentMap)
{
	continentMap.clear();
	vector<Object*> continentObjs = obj->getLeaves();	// the continents
	for (auto continentObj: continentObjs)
	{
		string continent = continentObj->getKey();	// the current continent
		vector<Object*> provinceObjs = continentObj->getValue("provinces");	// the province numbers in this continent
		for (auto provinceStr: provinceObjs[0]->getTokens())
		{
			const int province = atoi(provinceStr.c_str());	// the current province num
			continentMap.insert( make_pair(province, continent) );
		}
	}
}


void mergeNations(V2World& world, Object* mergeObj)
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
		V2Country* master = world.getCountry(masterTag);	// the actual master country
		if ( enabled && (master != NULL) && (slaveTags.size() > 0) )
		{
			for (vector<string>::iterator sitr = slaveTags.begin(); sitr != slaveTags.end(); ++sitr)
			{
				master->eatCountry(world.getCountry(*sitr));
			}
		}
	}
}

void removeEmptyNations(V2World& world)
{
	map<string, V2Country*> countries = world.getCountries();	// all V2 countries
	for (map<string, V2Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		map<int, V2Province*> provinces	= i->second->getProvinces();	// the provinces for the nation
		vector<V2Province*> cores			= i->second->getCores();		// the cores for the nation
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			world.removeCountry(i->first);
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
	vector<Object*> links = obj->getValue("link");
	for (vector<Object*>::iterator itr = links.begin(); itr != links.end(); ++itr)
	{
		GovernmentMap gov(*itr);
		governmentMap.push_back(gov);
	}

	return governmentMap;
}


void initUnionCultures(Object* obj, unionCulturesMap& unionCultures)
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


cultureMapping initCultureMap(Object* obj)
{
	cultureMapping cultureMap;						// the culture mapping
	vector<Object*> links = obj->getLeaves();	// the culture mapping rules

	for (vector<Object*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<Object*>	cultures	= (*i)->getLeaves();	// the items in this rule
		string				dstCulture;							// the HoI3 culture
		vector<string>		srcCulture;							// the Vic2 cultures

		for (vector<Object*>::iterator j = cultures.begin(); j != cultures.end(); j++)
		{
			if ( (*j)->getKey() == "hoi3" )
			{
				dstCulture = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "v2" )
			{
				srcCulture.push_back( (*j)->getLeaf() );
			}
		}

		for (vector<string>::iterator j = srcCulture.begin(); j != srcCulture.end(); j++)
		{
			cultureMap.insert(make_pair((*j), dstCulture));
		}
	}

	return cultureMap;
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


void initGovernmentJobTypes(Object* obj, governmentJobsMap& governmentJobs)
{
	vector<Object*> jobsObj = obj->getLeaves();
	for (auto jobsItr: jobsObj)
	{
		string job = jobsItr->getKey();
		vector<string> traits;
		auto traitsObj = jobsItr->getLeaves();
		for (auto trait: traitsObj)
		{
			traits.push_back(trait->getLeaf());
		}
		governmentJobs.insert(make_pair(job, traits));
	}
}


void initLeaderTraitsMap(Object* obj, leaderTraitsMap& leaderTraits)
{
	vector<Object*> typesObj = obj->getLeaves();
	for (auto typeItr: typesObj)
	{
		string type = typeItr->getKey();
		vector<string> traits;
		auto traitsObj = typeItr->getLeaves();
		for (auto trait: traitsObj)
		{
			traits.push_back(trait->getLeaf());
		}
		leaderTraits.insert(make_pair(type, traits));
	}
}


void initLeaderPersonalityMap(Object* obj, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap)
{
	vector<Object*> personalitiesObj = obj->getLeaves();
	for (auto personalityItr: personalitiesObj)
	{
		string personality = personalityItr->getKey();
		vector<string> landTraits;
		vector<string> seaTraits;
		auto traitsObj = personalityItr->getLeaves();
		for (auto trait: traitsObj)
		{
			if (trait->getKey() == "land")
			{
				landTraits.push_back(trait->getLeaf());
			}
			else if (trait->getKey() == "sea")
			{
				seaTraits.push_back(trait->getLeaf());
			}
		}
		landPersonalityMap.insert(make_pair(personality, landTraits));
		seaPersonalityMap.insert(make_pair(personality, seaTraits));
	}
}


void initLeaderBackgroundMap(Object* obj, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	vector<Object*> backgroundObj = obj->getLeaves();
	for (auto backgroundItr: backgroundObj)
	{
		string background = backgroundItr->getKey();
		vector<string> landTraits;
		vector<string> seaTraits;
		auto traitsObj = backgroundItr->getLeaves();
		for (auto trait: traitsObj)
		{
			if (trait->getKey() == "land")
			{
				landTraits.push_back(trait->getLeaf());
			}
			else if (trait->getKey() == "sea")
			{
				seaTraits.push_back(trait->getLeaf());
			}
		}
		landBackgroundMap.insert(make_pair(background, landTraits));
		seaBackgroundMap.insert(make_pair(background, seaTraits));
	}
}


void initNamesMapping(Object* obj, namesMapping& namesMap)
{
	vector<Object*> groupsObj = obj->getLeaves();
	for (auto groupsItr: groupsObj)
	{
		vector<Object*> culturesObj = groupsItr->getLeaves();
		for (auto culturesItr: culturesObj)
		{
			string key = culturesItr->getKey();
			if ((key == "union") || (key == "leader") || (key == "unit") || (key == "is_overseas"))
			{
				continue;
			}
			vector<Object*>	firstNamesObj	= culturesItr->getValue("first_names");
			vector<Object*>	lastNamesObj	= culturesItr->getValue("last_names");
			if ((firstNamesObj.size() > 0) && (lastNamesObj.size() > 0))
			{
				vector<string>		firstNames		= firstNamesObj[0]->getTokens();
				vector<string>		lastNames		= lastNamesObj[0]->getTokens();
				namesMap.insert(make_pair(key, make_pair(firstNames, lastNames)));
			}
			else
			{
				LOG(LogLevel::Error) << "No names for " << key;
			}	
		}
	}
}


void initPortraitMapping(Object* obj, portraitMapping& portraitMap)
{
	vector<Object*> groupsObj = obj->getLeaves();
	for (auto groupsItr: groupsObj)
	{
		vector<string> portraits = groupsItr->getTokens();
		portraitMap.insert(make_pair(groupsItr->getKey(), portraits));
	}
}


void initAIFocusModifiers(Object* obj, AIFocusModifiers& modifiers)
{
	vector<Object*> focusesObj = obj->getLeaves();
	for (auto focusesItr: focusesObj)
	{
		pair<AIFocusType, vector<AIFocusModifier>> newFocus;
		string focusName = focusesItr->getKey();
		if (focusName == "sea_focus")
		{
			newFocus.first = SEA_FOCUS;
		}
		else if (focusName == "tank_focus")
		{
			newFocus.first = TANK_FOCUS;
		}
		else if (focusName == "air_focus")
		{
			newFocus.first = AIR_FOCUS;
		}
		else if (focusName == "inf_focus")
		{
			newFocus.first = INF_FOCUS;
		}

		vector<Object*> modifiersObj = focusesItr->getLeaves();
		for (auto modifiersItr: modifiersObj)
		{
			AIFocusModifier newModifier;

			vector<Object*> modifierItems = modifiersItr->getLeaves();

			if (modifierItems.size() > 0)
			{
				string factorStr = modifierItems[0]->getLeaf();
				newModifier.modifierAmount = atof(factorStr.c_str());
			}
			if (modifierItems.size() > 1)
			{
				newModifier.modifierType			= modifierItems[1]->getKey();
				newModifier.modifierRequirement	= modifierItems[1]->getLeaf();
			}

			newFocus.second.push_back(newModifier);
		}

		modifiers.insert(newFocus);
	}
}