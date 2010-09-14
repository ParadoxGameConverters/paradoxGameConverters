// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************

// standard includes
#include "stdafx.h"
#include "Mapper.h"
#include "Logger.h"

#include <algorithm>

void Mapper::MapProvinces(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld)
{
   std::ostringstream stream;
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

   // Now output information on which provinces remain unmapped
   stream.str("");
   stream << "Mapper::MapProvinces finished mapping provinces.";
   Logger::WriteLine(stream.str());

   int totalUnmatched = 0;
   std::vector<Province*> provinces = destWorld.GetAllProvinces();

   stream.str("");
   for (unsigned int i = 0; i < provinces.size(); i++)
   {
      if (provinces[i]->GetSourceProvinces().size() < 1)
      {	 
	 if (stream.str().size() < 1)
	 {
	    stream << "Mapper::MapProvinces found destination province(s) with no source: ";
	 }
	 else
	 {
	    stream << ", ";
	 }
	 stream << provinces[i]->GetName();	 	 
	 totalUnmatched++;
      }
   }
   if (totalUnmatched > 0)
   {
      Logger::WriteLine(stream.str());

      stream.str("");
      stream << "Mapper::MapProvinces found " << totalUnmatched << " unmatched provinces.";
      Logger::WriteLine(stream.str());
   }   
}

void Mapper::MapCountries(std::map<std::string, std::set<std::string> > mapping, World& origWorld, World& destWorld)
{
   std::ostringstream stream;
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

   // Now output information on which countries remain unmapped
   stream.str("");
   stream << "Mapper::MapCountries finished mapping countries.";
   Logger::WriteLine(stream.str());

   int totalUnmatched = 0;
   std::vector<Country*> countries = origWorld.GetAllCountries();

   stream.str("");
   for (unsigned int i = 0; i < countries.size(); i++)
   {
      if (countries[i]->GetDestCountry() == NULL)
      {
	 if (stream.str().size() < 1)
	 {
	    stream << "Mapper::MapCountries found countries with no destination: ";
	 }
	 else
	 {
	    stream << ", ";
	 }
	 stream << countries[i]->GetName();	 	 
	 totalUnmatched++; 
      }
   }

   if (totalUnmatched > 0)
   {
      Logger::WriteLine(stream.str());

      stream.str("");
      stream << "Mapper::MapCountries found " << totalUnmatched << " unmatched countries.";
      Logger::WriteLine(stream.str());
   }
}


void Mapper::AssignProvinceOwnership(World& origWorld, World& destWorld, RegionListing& regionListing)
{
   std::vector<Object*> ownerVal;
   std::vector<Province*> allProvinces = destWorld.GetAllProvinces();

   for (unsigned int i = 0; i < allProvinces.size(); i++)
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

      // Now, re-identify the current owner, and assign a member variable
      ownerVal = allProvinces[i]->GetSource()->getValue("owner");
      if (ownerVal.size() > 0)
      {
	 std::string ownerStr = ownerVal[0]->getLeaf().substr(1, 3);
	 Country* owner = destWorld.GetCountry(ownerStr);
	 owner->AddProvince(allProvinces[i]);
      }
   }
}

void Mapper::SetupStates(World& destWorld, RegionListing& regionListing)
{
   int startState = 590;
   std::vector<Country*> allCountries = destWorld.GetAllCountries();

   for (unsigned int i = 0; i < allCountries.size(); i++)
   {
      Country* country = allCountries[i];
      std::vector<Province*> provinces = country->GetProvinces();
      std::vector<std::string> provinceIDs;

      for (unsigned int i = 0; i < provinces.size(); i++)
      {
	 provinceIDs.push_back(provinces[i]->GetName());
      }

      while (provinceIDs.size() > 0)
      {	 
	 std::vector<std::string>::iterator iter = provinceIDs.begin();
	 std::vector<std::string> state;

	 state.push_back((*iter));
	 provinceIDs.erase(iter);

	 std::vector<std::string> neighbours = regionListing.GetProvincesInSameState(state[0]);

	 for (unsigned int i = 0; i < neighbours.size(); i++)
	 {
	    iter = std::find(provinceIDs.begin(), provinceIDs.end(), neighbours[i]);
	    if (iter != provinceIDs.end())
	    {
	       state.push_back(*iter);
	       provinceIDs.erase(iter);
	    }
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

   for (unsigned int i = 0; i < data.size(); i++)
   {
      std::string euID;
      std::vector<std::string> vickyIDs;

      std::vector<Object*> euMaps = data[i]->getLeaves();

      for (unsigned int j = 0; j < euMaps.size(); j++)
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
      for (unsigned int j = 0; j < vickyIDs.size(); j++)
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
