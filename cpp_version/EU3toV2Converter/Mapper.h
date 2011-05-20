// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************


#ifndef MAPPER_H
#define MAPPER_H


#include "EU3World.h"


typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
provinceMapping initProvinceMap(Object* obj);

typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>
int initCountryMap(countryMapping& mapping, vector<string> EU3Tags, vector<string> V2Tags, Object* rulesObj);
vector<string>	getEU3Tags(EU3World srcWorld);
void removeEmptyNations(EU3World&);
void removeDeadLandlessNations(EU3World&);
void removeOlderLandlessNations(EU3World&, int&);
void removeLandlessNations(EU3World&);

typedef map< int, vector<int> > stateMapping;	// < province, all other provinces in state >
stateMapping initStateMap(Object* obj);

typedef map< string, string > cultureMapping;		// <srcCulture, destCulture>
cultureMapping initCultureMap(Object* obj);

typedef map< string, string> religionMapping;
religionMapping initReligionMap(Object* obj);		// <srcReligion, destReligion>


#endif // MAPPER_H
