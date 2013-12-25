#include "GovernmentMapper.h"

#include <math.h>
#include <float.h>
#include "Log.h"

static vector<GovernmentMap> governmentMapping;

GovernmentMap::GovernmentMap(Object* link)
{
	vector<Object*> vicName = link->getValue("vic");
	if (vicName.size() > 0)
	{
		vic_gov = vicName[0]->getLeaf();
	}

	vector<Object*> hoiName = link->getValue("hoi");
	if (hoiName.size() > 0)
	{
		hoi3name = hoiName[0]->getLeaf();
	}
	else
	{
		log("Warning: Malformed government map (no \"hoi\" leaf)\n");
	}

	vector<Object*> culture = link->getValue("culture");
	if (culture.size() > 0)
	{
		require_culture = culture[0]->getLeaf();
	}

	vector<Object*> rulingParty = link->getValue("ruling_party");
	if (rulingParty.size() > 0)
	{
		require_ruling_party = rulingParty[0]->getLeaf();
	}

	closest_uh_social = 0.0;
	vector<Object*> uhSocial = link->getValue("closest_uh_social");
	if (uhSocial.size() > 0)
	{
		closest_uh_social = atof(uhSocial[0]->getLeaf().c_str());
	}

	closest_uh_market = 0.0;
	vector<Object*> uhMarket = link->getValue("closest_uh_social");
	if (uhMarket.size() > 0)
	{
		closest_uh_market = atof(uhMarket[0]->getLeaf().c_str());
	}
}

double GovernmentMap::score(V2Country& country, const V2World& world, double avgSocial, double avgMarket)
{
	double result = 0.0;

	// reject mappings with bad names
	if (hoi3name.size() < 1)
	{
		return -1000000.0;
	}

	// if a government, culture, or ruling party is required, add a small amount if met, or deduct a large amount if not
	// in other words: all other things being equal, prefer the link with stricter requirements
	if (vic_gov.size() > 0)
	{
		if (vic_gov == country.getGovernmentType())
			result += 0.001;
		else
			result -= 1000.0;
	}
	if (require_culture.size() > 0)
	{
		if (require_culture == country.getPrimaryCulture())
			result += 0.001;
		else
			result -= 1000.0;
	}
	if (require_ruling_party.size() > 0)
	{
		if (require_ruling_party == world.getParty(country.getRulingParty()).ideology)
			result += 0.001;
		else
			result -= 1000.0;
	}

	// now, calculate distances for closest match, and subtract from result (higher score = better match)
	double dSocial = avgSocial - closest_uh_social;
	double dMarket = avgMarket - closest_uh_market;
	result -= sqrt(dSocial * dSocial + dMarket * dMarket);

	return result;
}

void initGovernmentMap(Object* mappings)
{
	vector<Object*> links = mappings->getValue("link");
	for (vector<Object*>::iterator itr = links.begin(); itr != links.end(); ++itr)
	{
		GovernmentMap gov(*itr);
		governmentMapping.push_back(gov);
	}
}

string getGovernmentForCountry(V2Country& country, const V2World& world)
{
	// initialize 2D UH ideology positions
	// these should probably be read from file; oh well.
	map<string, double> social_positions;
	map<string, double> market_positions;
	social_positions["reactionary"] = 2;		market_positions["reactionary"] = 1;
	social_positions["conservative"] = 1;		market_positions["conservative"] = 0;
	social_positions["fascist"] = 2;			market_positions["fascist"] = 1;
	social_positions["liberal"] = -1;			market_positions["liberal"] = 0;
	social_positions["anarcho_liberal"] = -2;	market_positions["anarcho_liberal"] = -2;
	social_positions["socialist"] = 0;			market_positions["socialist"] = 1;
	social_positions["communist"] = 1;			market_positions["communist"] = 2;

	// find the average political composition of the upper house
	double avgMarket = 0.0;
	double avgSocial = 0.0;
	for (map<string, double>::iterator itr = social_positions.begin(); itr != social_positions.end(); ++itr)
	{
		avgSocial += social_positions[itr->first] * country.getUpperHousePercentage(itr->first);
		avgMarket += market_positions[itr->first] * country.getUpperHousePercentage(itr->first);
	}

	log("%s Upper House Political Centre: Social: %f\tMarket: %f\n", country.getTag().c_str(), avgSocial, avgMarket);

	// do the mapping
	GovernmentMap* best = NULL;
	double best_val = -DBL_MAX;
	for (vector<GovernmentMap>::iterator itr = governmentMapping.begin(); itr != governmentMapping.end(); ++itr)
	{
		double this_val = itr->score(country, world, avgMarket, avgSocial);
		if (this_val > best_val)
		{
			best = &(*itr);
			best_val = this_val;
		}
	}

	log("Mapped %s government %s to %s (score: %f)\n", country.getTag().c_str(), country.getGovernmentType().c_str(), best->getName().c_str(), best_val);

	return best->getName();
}
