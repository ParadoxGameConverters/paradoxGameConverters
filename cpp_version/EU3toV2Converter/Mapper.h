// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef MAPPER_H
#define MAPPER_H

#include "stdafx.h"
#include "World.h"
#include "RegionListing.h"
#include "Parsers\Object.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Mapper
{
public:
   std::map<std::string, std::set<std::string> > InitEUToVickyMap(Object* obj);
   std::map<std::string, std::set<std::string> > InitEUToVickyCountryMap(Object* obj);
   void MapProvinces(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);
   void MapCountries(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);

   void AssignProvinceOwnership(World& origWorld, World& destWorld, RegionListing& regionListing);
   void SetupStates(World& destWorld, RegionListing& regionListing);
private:   
   void CreateState(std::string stateID, Country* country, std::vector<std::string>& provinces);
};

#endif // ndef MAPPER_H
