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
#include "Parsers\Object.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Mapper
{
public:
   std::map<std::string, std::set<std::string> > InitEUToVickyMap(Object* obj);
   void MapProvinces(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);
   void MapCountries(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld);

   void AssignProvinceOwnership(World& origWorld, World& destWorld);
   
private:   

};

#endif // ndef MAPPER_H
