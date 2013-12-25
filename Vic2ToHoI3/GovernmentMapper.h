#ifndef GOVERNMENTMAPPER_H
#define GOVERNMENTMAPPER_H

#include "Parsers/Object.h"
#include "V2Country.h"
#include "V2World.h"

class GovernmentMap
{
public:
	GovernmentMap(Object* link);
	double score(V2Country& country, const V2World& world, double avgMarket, double avgSocial);
	string getName() const { return hoi3name; };

private:
	string hoi3name;
	string vic_gov;
	string require_culture;
	string require_ruling_party;
	double closest_uh_social;
	double closest_uh_market;
};

void initGovernmentMap(Object* mappings);
string getGovernmentForCountry(V2Country& country, const V2World& world);

#endif // GOVERNMENTMAPPER_H