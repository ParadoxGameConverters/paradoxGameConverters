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

#include "GovernmentMapper.h"

#include <math.h>
#include <float.h>
#include "V2World/V2Country.h"
#include "Log.h"

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
		LOG(LogLevel::Warning) << "Malformed government map (no \"hoi\" leaf)";
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

double GovernmentMap::score(const V2Country* country, const string _ideology, double avgSocial, double avgMarket)
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
		if (vic_gov == country->getGovernment())
			result += 0.001;
		else
			result -= 1000.0;
	}
	if (require_culture.size() > 0)
	{
		if (require_culture == country->getPrimaryCulture())
			result += 0.001;
		else
			result -= 1000.0;
	}
	if (require_ruling_party.size() > 0)
	{
		if (require_ruling_party == _ideology)
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


string getGovernmentForCountry(const V2Country* country, const string _ideology, governmentMapping &governmentMap)
{
	// initialize 2D UH ideology positions
	// these should probably be read from file; oh well.
	map<string, double> social_positions;
	map<string, double> market_positions;
	social_positions["reactionary"]		=  2;	market_positions["reactionary"]		=  1;
	social_positions["conservative"]		=  1;	market_positions["conservative"]		=  0;
	social_positions["fascist"]			=  2;	market_positions["fascist"]			=  1;
	social_positions["liberal"]			= -1;	market_positions["liberal"]			=  0;
	social_positions["anarcho_liberal"]	= -2;	market_positions["anarcho_liberal"]	= -2;
	social_positions["socialist"]			=  0;	market_positions["socialist"]			=  1;
	social_positions["communist"]			=  1;	market_positions["communist"]			=  2;

	// find the average political composition of the upper house
	double avgMarket = 0.0;
	double avgSocial = 0.0;
	for (map<string, double>::iterator itr = social_positions.begin(); itr != social_positions.end(); ++itr)
	{
		avgSocial += social_positions[itr->first] * country->getUpperHousePercentage(itr->first);
		avgMarket += market_positions[itr->first] * country->getUpperHousePercentage(itr->first);
	}

	LOG(LogLevel::Debug) << country->getTag() << " Upper House Political Centre: Social: " << avgSocial << "\tMarket: " << avgMarket;

	// do the mapping
	GovernmentMap* best = NULL;
	double best_val = -DBL_MAX;
	for (vector<GovernmentMap>::iterator itr = governmentMap.begin(); itr != governmentMap.end(); ++itr)
	{
		double this_val = itr->score(country, _ideology, avgMarket, avgSocial);
		if (this_val > best_val)
		{
			best = &(*itr);
			best_val = this_val;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " government " << country->getGovernment() << " to " << best->getName() << "(score: " << best_val << ")";

	return best->getName();
}
