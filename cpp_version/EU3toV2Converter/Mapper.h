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
#include "Parsers\Object.h"
using namespace std;


typedef map< int, vector<int> > provinceMapping; // < destProvince, sourceProvinces >
provinceMapping initProvinceMap(Object* obj);

typedef map< string, string > countryMapping;	// < sourceCountry, destCountry>
countryMapping initCountryMap(Object* obj);

typedef map< int, vector<int> > stateMapping;	// < province, all other provinces in state >
stateMapping initStateMap(Object* obj);


#endif // ndef MAPPER_H
