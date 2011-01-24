// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef MAPPER_H
#define MAPPER_H

#include "stdafx.h"
#include <vector>
#include <string>
#include "World.h"
#include "RegionListing.h"
#include "Parsers\Object.h"
using namespace std;

std::map<std::string, std::set<std::string> > InitEUToVickyMap(Object* obj);
std::map<std::string, std::set<std::string> > InitEUToVickyCountryMap(Object* obj);
void MapProvinces(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);
void MapCountries(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);

void AssignProvinceOwnership(World& origWorld, World& destWorld, RegionListing& regionListing);
void AssignCountryCapitals(World& origWorld, World& destWorld);
void SetupStates(World& destWorld, RegionListing& regionListing);

typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
provinceMapping initProvinceMap(Object* obj);

typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>
countryMapping initCountryMap(Object* obj);

typedef map< int, vector<int> > stateMapping;
stateMapping initStateMap(Object* obj);


#endif // ndef MAPPER_H
