// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "Mapper.h"

void Mapper::MapProvinces(std::vector<oneToMany> mapping, World& origWorld, World& destWorld)
{
   for (int i = 0; i < mapping.size(); i++)
   {
      std::vector<Province*> sourceProvinces;
      Province* destProvince = destWorld.GetProvince(mapping[i].key);

      if (NULL == destProvince)
	 continue; // TODO - Warning

      for (int j = 0; j < mapping[i].values.size(); j++)
      {
	 Province* origProvince = origWorld.GetProvince(mapping[i].values[j]);

	 if (NULL == origProvince)
	    continue; // TODO - Warning

	 sourceProvinces.push_back(origProvince);
      }

      destProvince->SetSourceProvinces(sourceProvinces);
   }
}

void Mapper::AssignProvinceOwnership(World& destWorld)
{
   std::vector<Province*> allProvinces = destWorld.GetAllProvinces();

   for (int i = 0; i < allProvinces.size(); i++)
   {
      const std::vector<Province*> sourceProvinces = allProvinces[i]->GetSourceProvinces();

      if (sourceProvinces.size() > 0)
      {
	 std::string ownerStr;
	 std::vector<Object*> oldOwnerVal = sourceProvinces[0]->GetSource()->getValue("owner");
	 std::vector<Object*> newOwnerVal = allProvinces[i]->GetSource()->getValue("owner");

	 if ((oldOwnerVal.size() > 0) && (newOwnerVal.size() > 0))
	 {
	    ownerStr = oldOwnerVal[0]->getLeaf();
	    newOwnerVal[0]->setLeaf(oldOwnerVal[0]->getKey(), ownerStr);
	 }
      }
   }
}

