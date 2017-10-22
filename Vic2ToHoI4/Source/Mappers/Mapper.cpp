/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "../Configuration.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "../V2World/V2World.h"
#include <algorithm>



HoI4AdjacencyMapping initHoI4AdjacencyMap()
{
	LOG(LogLevel::Info) << "Importing HoI4 adjacencies";
	//FILE* adjacenciesBin = NULL;	// the adjacencies.bin file
	//string filename = Configuration::getHoI4Path() + "/tfh/map/cache/adjacencies.bin";
	//fopen_s(&adjacenciesBin, filename.c_str(), "rb");
	//if (adjacenciesBin == NULL)
	//{
	//	LOG(LogLevel::Error) << "Could not open " << filename;
	//	exit(1);
	//}

	HoI4AdjacencyMapping adjacencyMap;	// the adjacency mapping
	//while (!feof(adjacenciesBin))
	//{
	//	int numAdjacencies;	// the total number of adjacencies
	//	if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesBin) != 1)
	//	{
	//		break;
	//	}
	//	vector<adjacency> adjacencies;	// the adjacencies for the current province
	//	for (int i = 0; i < numAdjacencies; i++)
	//	{
	//		adjacency newAdjacency;	// the current adjacency
	//		fread(&newAdjacency, sizeof(newAdjacency), 1, adjacenciesBin);
	//		adjacencies.push_back(newAdjacency);
	//	}
	//	adjacencyMap.push_back(adjacencies);
	//}
	//fclose(adjacenciesBin);

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


void initContinentMap(shared_ptr<Object> obj, continentMapping& continentMap)
{
	continentMap.clear();
	vector<shared_ptr<Object>> continentObjs = obj->getLeaves();	// the continents
	for (auto continentObj: continentObjs)
	{
		string continent = continentObj->getKey();	// the current continent
		vector<shared_ptr<Object>> provinceObjs = continentObj->getValue("provinces");	// the province numbers in this continent
		for (auto provinceStr: provinceObjs[0]->getTokens())
		{
			const int province = stoi(provinceStr);	// the current province num
			continentMap.insert( make_pair(province, continent) );
		}
	}
}


unionMapping initUnionMap(shared_ptr<Object> obj)
{
	unionMapping unionMap;	// the cultural unions map

	vector<shared_ptr<Object>> unions = obj->getLeaves();	// the rules for cultural unions
	for (auto aUnion: unions)
	{
		string tag;		// the tag for the cultural union
		string culture;	// the culture for the cultural union

		for (auto item: aUnion->getLeaves())
		{
			if (item->getKey() == "tag")
			{
				tag = item->getLeaf();
			}
			if (item->getKey() == "culture")
			{
				culture = item->getLeaf();
			}
		}

		unionMap.push_back(make_pair(culture, tag));
	}

	return unionMap;
}


void initUnionCultures(shared_ptr<Object> obj, unionCulturesMap& unionCultures)
{
	vector<shared_ptr<Object>> cultureGroups = obj->getLeaves();	// the cultural group rules
	for (auto cultureGroup: cultureGroups)
	{
		vector<shared_ptr<Object>>		culturesObj		= cultureGroup->getLeaves();	// the items in this rule
		string					group				= cultureGroup->getKey();		// the cultural group
		vector<string>		cultures;												// the cultures

		for (auto item: culturesObj)
		{
			if (item->getKey() == "dynasty_names")
			{
				continue;
			}
			else if (item->getKey() == "graphical_culture")
			{
				continue;
			}
			else
			{
				cultures.push_back(item->getKey());
			}
		}

		unionCulturesMap::iterator itr = unionCultures.find(group);
		if (itr != unionCultures.end())
		{
			vector<string> oldCultures = itr->second;	// any cultures already in the group
			for (auto jtr: oldCultures)
			{
				cultures.push_back(jtr);
			}
		}
		unionCultures[group] = cultures;
	}
}


void initIdeaEffects(shared_ptr<Object> obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas)
{
	vector<shared_ptr<Object>> ideasObj = obj->getLeaves();
	for (auto ideasItr: ideasObj)
	{
		string idea = ideasItr->getKey();
		vector<shared_ptr<Object>> effects = ideasItr->getLeaves();
		for (auto effectsItr: effects)
		{
			string effectType = effectsItr->getKey();
			if (effectType == "army_investment")
			{
				armyInvIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "commerce_investment")
			{
				commerceInvIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "culture_investment")
			{
				cultureInvIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "industry_investment")
			{
				industryInvIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "navy_investment")
			{
				navyInvIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "upper_house_liberal")
			{
				UHLiberalIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "upper_house_reactionary")
			{
				UHReactionaryIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "NV_order")
			{
				orderIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "NV_liberty")
			{
				libertyIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "NV_equality")
			{
				equalityIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "literacy")
			{
				vector<string> literacyStrs = effectsItr->getTokens();
				for (auto literacyString: literacyStrs)
				{
					literacyIdeas.push_back(make_pair(idea, stoi(literacyString)));
				}
			}
		}
	}
}


void initGovernmentJobTypes(governmentJobsMap& governmentJobs)
{
	LOG(LogLevel::Info) << "Parsing government jobs";
	shared_ptr<Object> obj = parser_UTF8::doParseFile("governmentJobs.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentJobs.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> jobsObj = obj->getLeaves();
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


void initLeaderTraitsMap(leaderTraitsMap& leaderTraits)
{
	LOG(LogLevel::Info) << "Parsing government jobs";

	shared_ptr<Object> obj = parser_UTF8::doParseFile("leader_traits.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file leader_traits.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> typesObj = obj->getLeaves();
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


void initLeaderPersonalityMap(personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap)
{
	LOG(LogLevel::Info) << "Parsing personality mappings";
	shared_ptr<Object> obj = parser_UTF8::doParseFile("personality_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file personality_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse personality_map.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> personalitiesObj = obj->getLeaves();
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


void initLeaderBackgroundMap(backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	LOG(LogLevel::Info) << "Parsing background mappings";
	shared_ptr<Object> obj = parser_UTF8::doParseFile("background_map.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file background_map.txt";
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse background_map.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> backgroundObj = obj->getLeaves();
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


void initPortraitMapping(portraitMapping& portraitMap)
{
	LOG(LogLevel::Info) << "Parsing portraits list";
	shared_ptr<Object> obj = parser_UTF8::doParseFile("portraits.txt");
	if (obj != NULL)
	{
		vector<shared_ptr<Object>> groupsObj = obj->getLeaves();
		for (auto groupsItr: groupsObj)
		{
			vector<string> portraits = groupsItr->getTokens();
			portraitMap.insert(make_pair(groupsItr->getKey(), portraits));
		}
	}
}