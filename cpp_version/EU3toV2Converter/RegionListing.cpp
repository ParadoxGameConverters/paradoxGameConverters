// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "RegionListing.h"
#include "Logger.h"

std::vector<std::string> RegionListing::GetProvincesInSameState(std::string provinceId)
{
   std::vector<std::string> blanks;
   std::map<std::string, std::vector<std::string> >::iterator theIter = m_regionMap.find(provinceId);

   if (theIter != m_regionMap.end())
   {
      return theIter->second;
   }

   return blanks;
}

void RegionListing::Init(Object* obj)
{
   std::vector<Object*> leaves = obj->getLeaves();

   // For each region
   for (unsigned int i = 0; i < leaves.size(); i++)
   {
      std::vector<std::string> neighbours = leaves[i]->getTokens();

      for (unsigned int j = 0; j < neighbours.size(); j++)
      {
	 m_regionMap.insert(std::make_pair<std::string, std::vector<std::string> >(neighbours[j], neighbours));
      }
   }
}