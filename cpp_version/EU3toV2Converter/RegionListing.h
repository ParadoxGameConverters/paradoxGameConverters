// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

#pragma once

#ifndef REGIONLISTING_H
#define REGIONLISTING_H

#include "stdafx.h"
#include "World.h"
#include "Parsers\Object.h"

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class RegionListing
{
public:
   void Init(Object* obj);
   std::vector<std::string> GetProvincesInSameState(std::string provinceId);
private:   
   std::map<std::string, std::vector<std::string> > m_regionMap;
};

#endif // ndef MAPPER_H
