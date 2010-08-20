// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "Mapper.h"

void Mapper::MapProvinces(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld)
{
   std::map<std::string, std::set<std::string> >::iterator mapIter;
   std::set<std::string>::iterator setIter;

   for (mapIter = mapping.begin(); mapIter != mapping.end(); mapIter++)
   {
      std::vector<Province*> sourceProvinces;
      Province* destProvince = destWorld.GetProvince((*mapIter).first);

      if (NULL == destProvince)
	 continue; // TODO - Warning

      for (setIter = (*mapIter).second.begin(); setIter != (*mapIter).second.end(); setIter++)
      {
	 Province* origProvince = origWorld.GetProvince(*setIter);

	 if (NULL == origProvince)
	    continue; // TODO - Warning

	 sourceProvinces.push_back(origProvince);
      }

      destProvince->SetSourceProvinces(sourceProvinces);
   }
}

void Mapper::MapCountries(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld)
{
   std::map<std::string, std::set<std::string> >::iterator mapIter;
   std::set<std::string>::iterator setIter;

   for (mapIter = mapping.begin(); mapIter != mapping.end(); mapIter++)
   {
      std::vector<Country*> sources;
      Country* dest = destWorld.GetCountry((*mapIter).first);

      if (NULL == dest)
	 continue; // TODO - Warning

      for (setIter = (*mapIter).second.begin(); setIter != (*mapIter).second.end(); setIter++)
      {
	 Country* orig = origWorld.GetCountry(*setIter);

	 if (NULL == orig)
	    continue; // TODO - Warning

	 sources.push_back(orig);
      }

      dest->SetSourceCountries(sources);
   }
}


void Mapper::AssignProvinceOwnership(World& origWorld, World& destWorld)
{
   std::vector<Province*> allProvinces = destWorld.GetAllProvinces();

   for (int i = 0; i < allProvinces.size(); i++)
   {
      const std::vector<Province*> sourceProvinces = allProvinces[i]->GetSourceProvinces();

      if (sourceProvinces.size() > 0)
      {
	 std::string ownerStr;
	 std::vector<Object*> oldOwnerVal, newOwnerVal; 
	 
	 oldOwnerVal = sourceProvinces[0]->GetSource()->getValue("owner");	 
	 if (oldOwnerVal.size() < 1)
	    continue;
	 
	 ownerStr = oldOwnerVal[0]->getLeaf();
	 ownerStr = ownerStr.substr(1, 3); // This was stored with quotes around it
	 Country* origCountry = origWorld.GetCountry(ownerStr);

	 if (origCountry == NULL)
	    continue;

	 Country* destCountry = origCountry->GetDestCountry();

	 if (destCountry == NULL)
	    continue;

	 ownerStr = "\"" + destCountry->GetName() + "\"";

	 newOwnerVal = allProvinces[i]->GetSource()->getValue("owner");
	 if (newOwnerVal.size() > 0)
	 {	    
	    newOwnerVal[0]->setValue(ownerStr);
	 }

	 newOwnerVal = allProvinces[i]->GetSource()->getValue("controller");
	 if (newOwnerVal.size() > 0)
	 {
	    newOwnerVal[0]->setValue(ownerStr);
	 }
      }
   }
}

std::map<std::string, std::set<std::string> > Mapper::InitEUToVickyMap(Object* obj)
{
   std::map<std::string, std::set<std::string> > mapping;
   std::map<std::string, std::set<std::string> >::iterator mapIter;
   std::set<std::string> blanks;

   std::vector<Object*> leaves = obj->getLeaves();
   
   if (leaves.size() < 1)
   {
      // TODO: Error
      return mapping;
   }
  
   std::vector<Object*> data = leaves[0]->getLeaves();

   for (int i = 0; i < data.size(); i++)
   {
      std::string euID;
      std::vector<std::string> vickyIDs;

      std::vector<Object*> euMaps = data[i]->getLeaves();

      for (int j = 0; j < euMaps.size(); j++)
      {
	 if (euMaps[j]->getKey().compare("eu3") == 0)
	 {
	    euID = euMaps[j]->getLeaf();
	 }
	 else if (euMaps[j]->getKey().compare("vic") == 0)
	 {
	    vickyIDs.push_back(euMaps[j]->getLeaf());
	 }
	 else
	 {
	    // Error
	 }
      }

      // Now convert to final result
      for (int j = 0; j < vickyIDs.size(); j++)
      {
	 mapIter = mapping.find(vickyIDs[j]);
	 if (mapIter == mapping.end())
	 {
	    mapping.insert(std::make_pair<std::string, std::set<std::string> >(vickyIDs[j], blanks));
	    mapIter = mapping.find(vickyIDs[j]);
	 }
	 (*mapIter).second.insert(euID);
      }
   }

   return mapping;
}
