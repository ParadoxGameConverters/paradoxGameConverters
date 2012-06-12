// ****************************************************************************
// *																								  *
// *			  EU3 to Victoria 2 conversion project							*
// *																								  *
// ****************************************************************************


#include "Mapper.h"
#include "Log.h"
#include "EU3Country.h"
#include "EU3Province.h"
#include <algorithm>


provinceMapping initProvinceMap(Object* obj)
{
	provinceMapping mapping;
	provinceMapping::iterator mapIter;
	//set<string> blanks;

	vector<Object*> leaves = obj->getLeaves();

	if (leaves.size() < 1)
	{
		log ("Error: No province mapping definitions loaded.\n");
		printf("Error: No province mapping definitions loaded.\n");
		return mapping;
	}

	vector<Object*> data = leaves[0]->getLeaves();

	for (unsigned int i = 0; i < data.size(); i++)
	{
		vector<int> EU3nums;
		vector<int> V2nums;

		vector<Object*> euMaps = data[i]->getLeaves();

		for (unsigned int j = 0; j < euMaps.size(); j++)
		{
			if (euMaps[j]->getKey().compare("eu3") == 0)
			{
				EU3nums.push_back(atoi(euMaps[j]->getLeaf().c_str()));
			}
			else if (euMaps[j]->getKey().compare("vic") == 0)
			{
				V2nums.push_back(atoi(euMaps[j]->getLeaf().c_str()));
			}
			else
			{
				log("Warning: unknown data while mapping provinces.\n");
			}
		}

		if (EU3nums.size() == 0)
		{
			EU3nums.push_back(0);
		}

		for (unsigned int k = 0; k < V2nums.size(); k++)
		{
			pair< int, vector<int> > insertMe;
			insertMe.first = V2nums[k];
			insertMe.second = EU3nums;
			mapping.insert(insertMe);
		}
	}

	return mapping;
}


// invert the sense of a province map.  makes army conversion tolerable.
inverseProvinceMapping invertProvinceMap(provinceMapping provinceMap)
{
	inverseProvinceMapping retval;
	for (provinceMapping::iterator j = provinceMap.begin(); j != provinceMap.end(); j++)
	{
		for (unsigned int k = 0; k < j->second.size(); k++)
		{
			retval[j->second[k]].push_back(j->first);
		}
	}
	return retval;
}

/*
vector<int> getV2ProvinceNums(provinceMapping provinceMap, int eu3ProvinceNum)
{
	vector<int> retval;
	for (provinceMapping::iterator j = provinceMap.begin(); j != provinceMap.end(); j++)
	{
		for (unsigned int k = 0; k < j->second.size(); k++)
		{
			if (j->second[k] == eu3ProvinceNum)
			{
				retval.push_back(j->first);
			}
		}
	}
	return retval;
}
*/

static const vector<int> empty_vec;
const vector<int>& getV2ProvinceNums(const inverseProvinceMapping& invProvMap, int eu3ProvinceNum)
{
	inverseProvinceMapping::const_iterator itr = invProvMap.find(eu3ProvinceNum);
	if (itr == invProvMap.end())
		return empty_vec;
	else
		return itr->second;
}


vector<string> processBlockedNations(Object* obj)
{
	vector<string> blockedNations;

	vector<Object*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		return blockedNations;
	}

	vector<Object*> nations = leaves[0]->getLeaves();
	for (unsigned int i = 0; i < nations.size(); i++)
	{
		blockedNations.push_back(nations[i]->getLeaf());
	}

	return blockedNations;
}


int initCountryMap(countryMapping& mapping, vector<string> EU3Tags, vector<string> V2Tags, vector<string> blockedNations, Object* rulesObj)
{
	mapping.clear();
	countryMapping::iterator mapIter;

	// get rules
	vector<Object*>		leaves = rulesObj->getLeaves();
	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return -1;
	}
	vector<Object*> rules = leaves[0]->getLeaves();

	for (unsigned int i = 0; i < rules.size(); ++i)
	{
		vector<Object*> rule = rules[i]->getLeaves();
		string			rEU3Tag;
		vector<string>	rV2Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("eu3") == 0)
			{		 
				rEU3Tag = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("vic") == 0)
			{
				rV2Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries.\n");
			}
		}

		//find EU3 tag from the rule
		vector<string>::iterator EU3TagPos = EU3Tags.end();
		for (vector<string>::iterator j = EU3Tags.begin(); j != EU3Tags.end(); j++)
		{
			if (*j == rEU3Tag)
			{
				EU3TagPos = j;
				break;
			}
		}
		if (EU3TagPos == EU3Tags.end())
		{
			continue;
		}

		//find V2 tag from the rule
		vector<string>::iterator V2TagPos = V2Tags.end();
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rV2Tags.rbegin(); j != rV2Tags.rend(); ++j)
		{
			++distance;
			for (vector<string>::iterator k = V2Tags.begin(); k != V2Tags.end(); ++k)
			{
				if (*k == *j)
				{
					V2TagPos = k;
					break;
				}
			}
			if (V2TagPos != V2Tags.end())
			{
				break;
			}
		}
		if (V2TagPos == V2Tags.end())
		{
			continue;
		}

		//add the mapping
		mapping.insert(make_pair<string, string>(*EU3TagPos, *V2TagPos));
		log("	Added map %s -> %s (#%d)\n", EU3TagPos->c_str(), V2TagPos->c_str(), distance);

		//remove tags from the lists
		EU3Tags.erase(EU3TagPos);
		V2Tags.erase(V2TagPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		for (vector<string>::iterator i = V2Tags.begin(); i != V2Tags.end(); ++i)
		{
			if (*i == blockedNations[j])
			{
				V2Tags.erase(i);
				break;
			}
		}
	}

	while ( (EU3Tags.size() > 0) && (V2Tags.size() > 0) )
	{
		vector<string>::iterator EU3TagPos = EU3Tags.begin();
		if ( (*EU3TagPos == "REB") || (*EU3TagPos == "PIR") || (*EU3TagPos == "NAT") )
		{
			mapping.insert(make_pair<string, string>(*EU3TagPos, "REB"));
			EU3Tags.erase(EU3TagPos);
		}
		else
		{
			vector<string>::iterator V2TagPos = V2Tags.begin();
			mapping.insert(make_pair<string, string>(*EU3TagPos, *V2TagPos));
			log("	Added map %s -> %s (fallback)\n", EU3TagPos->c_str(), V2TagPos->c_str());

			//remove tags from the lists
			EU3Tags.erase(EU3TagPos);
			V2Tags.erase(V2TagPos);
		}
	}

	return EU3Tags.size();
}


void uniteJapan(EU3World& world)
{
	vector<EU3Country*> countries = world.getCountries();
	vector<EU3Country*>::iterator japan;
	for (japan = countries.begin(); japan != countries.end(); japan++)
	{
		if ( (*japan)->getTag() == "JAP")
		{
			break;
		}
	}
	if (japan == countries.end())
	{
		return;
	}
	vector<string> japanFlags = (*japan)->getFlags();
	for (unsigned int i = 0; i < japanFlags.size(); i++)
	{
		if (japanFlags[i] == "united_daimyos_of_japan")
		{
			return;
		}
	}

	vector<vector<EU3Country>::iterator> daimyos;
	for (vector<EU3Country*>::iterator i = countries.begin(); i != countries.end(); ++i)
	{
		if ( (*i)->getPossibleDaimyo() )
		{
			(*japan)->eatCountry(world.getCountry( (*i)->getTag() ));
		}
	}
}


void mergeNations(EU3World& world, Object* mergeObj)
{
	vector<Object*> rules = mergeObj->getValue("merge_nations");
	if (rules.size() < 0)
	{
		log("	No nations have merging requested (skipping).\n");
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
				enabled = true;
			else if ((*jtr)->getKey() == "master")
				masterTag = (*jtr)->getLeaf();
			else if ((*jtr)->getKey() == "slave")
				slaveTags.push_back((*jtr)->getLeaf());
		}
		EU3Country* master = world.getCountry(masterTag);
		if (enabled && master != NULL && slaveTags.size() > 0)
		{
			for (vector<string>::iterator sitr = slaveTags.begin(); sitr != slaveTags.end(); ++sitr)
			{
				master->eatCountry(world.getCountry(*sitr));
			}
		}
	}
}


void removeEmptyNations(EU3World& world)
{
	vector<EU3Country*> countries = world.getCountries();

	vector<string> tagsToRemove;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		vector<EU3Province*> provinces	= countries[i]->getProvinces();
		vector<EU3Province*> cores			= countries[i]->getCores();
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			tagsToRemove.push_back(countries[i]->getTag());
		}
	}
	world.removeCountries(tagsToRemove);
}


bool compareLandlessNationsAges(EU3Country* A, EU3Country* B)
{
	vector<EU3Province*> ACores = A->getCores();
	string ATag = A->getTag();
	date ADate;
	for (unsigned int i = 0; i < ACores.size(); i++)
	{
		date newADate	= ACores[i]->getLastPossessedDate(ATag);
		if (newADate > ADate)
		{
			ADate = newADate;
		}
	}

	vector<EU3Province*> BCores = B->getCores();
	string BTag = B->getTag();
	date BDate;
	for (unsigned int i = 0; i < BCores.size(); i++)
	{
		date newBDate	= BCores[i]->getLastPossessedDate(BTag);
		if (newBDate > BDate)
		{
			BDate = newBDate;
		}
	}

	return (ADate < BDate);
}


void removeDeadLandlessNations(EU3World& world)
{
	vector<EU3Country*> countries = world.getCountries();

	vector<EU3Country*> countries2;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		vector<EU3Province*> provinces = countries[i]->getProvinces();
		if (provinces.size() == 0)
		{
			countries2.push_back(countries[i]);
		}
	}

	vector<string> tagsToRemove;
	for (unsigned int i = 0; i < countries2.size(); i++)
	{
		string culture					= countries2[i]->getPrimaryCulture();
		vector<EU3Province*> cores	= countries2[i]->getCores();
		bool cultureSurvives			= false;
		for (unsigned int j = 0; j < cores.size(); j++)
		{
			if (cores[j]->getOwner() == "")
				continue;
			if (cores[j]->getCulture() == culture)
			{
				if (world.getCountry(cores[j]->getOwner())->getPrimaryCulture() != culture)
				{
					cultureSurvives = true;
					break;
				}	
			}
		}
		if (cultureSurvives == false)
		{
			tagsToRemove.push_back(countries2[i]->getTag());
		}
	}
	world.removeCountries(tagsToRemove);
}


void removeOlderLandlessNations(EU3World& world, int excess)
{
	vector<EU3Country*> countries = world.getCountries();

	vector<EU3Country*> countries2;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		vector<EU3Province*> provinces = countries[i]->getProvinces();
		if (provinces.size() == 0)
		{
			countries2.push_back(countries[i]);
		}
	}

	sort(countries2.begin(), countries2.end(), compareLandlessNationsAges);

	vector<string> tagsToRemove;
	while ( (excess > 0) && (countries2.size() > 0) )
	{
		tagsToRemove.push_back(countries2.back()->getTag());
		countries2.pop_back();
		excess--;
	}
	world.removeCountries(tagsToRemove);
}


void removeLandlessNations(EU3World& world)
{
	vector<EU3Country*> countries = world.getCountries();

	vector<string> tagsToRemove;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		vector<EU3Province*> provinces = countries[i]->getProvinces();
		if (provinces.size() == 0)
		{
			tagsToRemove.push_back(countries[i]->getTag());
		}
	}
	world.removeCountries(tagsToRemove);
}


vector<string> getEU3Tags(EU3World& srcWorld)
{
	vector<EU3Country*>	EU3Countries = srcWorld.getCountries();
	vector<string>			EU3Tags;

	for (unsigned int i = 0; i < EU3Countries.size(); i++)
	{
		EU3Tags.push_back(EU3Countries[i]->getTag());
	}

	return EU3Tags;
}


stateMapping initStateMap(Object* obj)
{
	stateMapping stateMap;
	vector<Object*> leaves = obj->getLeaves();

	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		vector<string> provinces = leaves[i]->getTokens();
		vector<int>		neighbors;

		for (unsigned int j = 0; j < provinces.size(); j++)
		{
			neighbors.push_back(atoi(provinces[j].c_str()));
		}
		for (unsigned int j = 0; j < neighbors.size(); j++)
		{
			stateMap.insert(make_pair<int, vector<int> >(neighbors[j], neighbors));
		}
	}

	return stateMap;
}


cultureMapping initCultureMap(Object* obj)
{
	cultureMapping cultureMap;
	vector<Object*> links = obj->getLeaves();

	for (unsigned int i = 0; i < links.size(); i++)
	{
		vector<Object*>			cultures	= links[i]->getLeaves();
		vector<string>				srcCultures;
		string						dstCulture;
		vector< distinguisher > distinguishers;

		for (unsigned int j = 0; j < cultures.size(); j++)
		{
			if (cultures[j]->getKey() == "vic")
			{
				dstCulture = cultures[j]->getLeaf();
			}
			if (cultures[j]->getKey() == "eu3")
			{
				srcCultures.push_back(cultures[j]->getLeaf());
			}
			if (cultures[j]->getKey() == "owner")
			{
				distinguisher newD;
				newD.first	= owner;
				newD.second	= cultures[j]->getLeaf();
				distinguishers.push_back(newD);
			}
			if (cultures[j]->getKey() == "religion")
			{
				distinguisher newD;
				newD.first	= religion;
				newD.second	= cultures[j]->getLeaf();
				distinguishers.push_back(newD);
			}
		}

		for (unsigned int j = 0; j < srcCultures.size(); j++)
		{
			cultureStruct	rule;
			rule.srcCulture		= srcCultures[j];
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

	for (unsigned int i = 0; i < links.size(); i++)
	{
		vector<Object*>	religions	= links[i]->getLeaves();
		string				dstReligion;
		vector<string>		srcReligion;

		for (unsigned int j = 0; j < religions.size(); j++)
		{
			if (religions[j]->getKey() == "vic")
			{
				dstReligion = religions[j]->getLeaf();
			}
			if (religions[j]->getKey() == "eu3")
			{
				srcReligion.push_back(religions[j]->getLeaf());
			}
		}

		for (unsigned int j = 0; j < srcReligion.size(); j++)
		{
			religionMap.insert(pair<string,string>(srcReligion[j], dstReligion));
		}
	}

	return religionMap;
}


unionMapping initUnionMap(Object* obj)
{
	unionMapping unionMap;

	vector<Object*> unions = obj->getLeaves();
	for (unsigned int i = 0; i < unions.size(); i++)
	{
		string tag;
		string culture;

		vector<Object*> aUnion = unions[i]->getLeaves();
		for (unsigned int j = 0; j < aUnion.size(); j++)
		{
			if (aUnion[j]->getKey() == "tag")
			{
				tag = aUnion[j]->getLeaf();
			}
			if (aUnion[j]->getKey() == "culture")
			{
				culture = aUnion[j]->getLeaf();
			}
		}

		unionMap.push_back(make_pair<string,string>(culture, tag));
	}

	return unionMap;
}


governmentMapping initGovernmentMap(Object* obj)
{
	governmentMapping governmentMap;
	vector<Object*> links = obj->getLeaves();

	for (unsigned int i = 0; i < links.size(); i++)
	{
		vector<Object*>	governments	= links[i]->getLeaves();
		string				dstGovernment;
		vector<string>		srcGovernments;

		for (unsigned int j = 0; j < governments.size(); j++)
		{
			if (governments[j]->getKey() == "vic")
			{
				dstGovernment = governments[j]->getLeaf();
			}
			if (governments[j]->getKey() == "eu3")
			{
				srcGovernments.push_back(governments[j]->getLeaf());
			}
		}

		for (unsigned int j = 0; j < srcGovernments.size(); j++)
		{
			governmentMap.insert(pair<string,string>(srcGovernments[j], dstGovernment));
		}
	}

	return governmentMap;
}


unionCulturesList initUnionCultures(Object* obj)
{
	unionCulturesList unionCultures;
	vector<Object*> cultureGroups = obj->getLeaves();

	for (unsigned int i = 0; i < cultureGroups.size(); i++)
	{
		vector<Object*>		cultures			= cultureGroups[i]->getLeaves();
		bool						hasUnion			= false;
		unionCultureStruct	unionCulture;

		for (unsigned int j = 0; j < cultures.size(); j++)
		{
			if (cultures[j]->getKey() == "union")
			{
				hasUnion = true;
				unionCulture.tag = cultures[j]->getLeaf();
			}
			else if (cultures[j]->getKey() == "dynasty_names")
			{
			}
			else
			{
				unionCulture.cultures.push_back(cultures[j]->getKey());
			}
		}

		if (hasUnion)
		{
			unionCultures.push_back(unionCulture);
		}
	}

	return unionCultures;
}