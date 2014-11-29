#ifndef GOVERNMENTMAPPER_H
#define GOVERNMENTMAPPER_H

#include "Parsers/Object.h"

class V2Country;

class GovernmentMap
{
public:
	GovernmentMap(Object* link);
	double score(const V2Country* country, const string _ideology, double avgMarket, double avgSocial);
	string getName() const { return hoi3name; };

private:
	string hoi3name;
	string vic_gov;
	string require_culture;
	string require_ruling_party;
	double closest_uh_social;
	double closest_uh_market;
};

//void initGovernmentMap(Object* mappings);
typedef vector<GovernmentMap> governmentMapping;
string getGovernmentForCountry(const V2Country* country, const string _ideology, governmentMapping &governmentMap);

#endif // GOVERNMENTMAPPER_H