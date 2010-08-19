// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef MAPPER_H
#define MAPPER_H

#include "World.h"
///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class Mapper
{
public:
   void MapProvinces(std::vector<oneToMany> mapping, World& origWorld, World& destWorld);
   void AssignProvinceOwnership(World& destWorld);
private:   

};

#endif // ndef MAPPER_H
