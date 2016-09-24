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
#include "EU4RegionMapper.h"
#include "Log.h"
#include "Object.h"
#include "../V2World/V2Localisation.h"



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
			else if ((*j)->getKey() == "male_names")
			{
				continue;
			}
			else if ((*j)->getKey() == "female_names")
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


void initIdeaEffects(Object* obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& armyTechIdeas, map<string, double>& commerceTechIdeas, map<string, double>& cultureTechIdeas, map<string, double>& industryTechIdeas, map<string, double>& navyTechIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas)
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
			else if (effectType == "army_tech_score")
			{
				armyTechIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "commerce_tech_score")
			{
				commerceTechIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "culture_tech_score")
			{
				cultureTechIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "industry_tech_score")
			{
				industryTechIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
			}
			else if (effectType == "navy_tech_score")
			{
				navyTechIdeas[idea] = atof((*effectsItr)[0].getLeaf().c_str());
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