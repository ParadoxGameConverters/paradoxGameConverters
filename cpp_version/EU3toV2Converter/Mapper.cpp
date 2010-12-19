// ****************************************************************************
// *																								  *
// *			  EU3 to Victoria 2 conversion project							*
// *																								  *
// ****************************************************************************


#include "stdafx.h"
#include "Mapper.h"
#include "Log.h"

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
		{
			log("Warning: Destination province is null.\n");
			printf("Warning: Destination province is null.\n");
			continue;
		}

		for (setIter = (*mapIter).second.begin(); setIter != (*mapIter).second.end(); setIter++)
		{
			Province* origProvince = origWorld.GetProvince(*setIter);

			if (NULL == origProvince)
			{
				log("Warning: Origination province is null.\n");
				printf("Warning: Origination province is null.\n");
				continue;
			}

			sourceProvinces.push_back(origProvince);
		}

		destProvince->SetSourceProvinces(sourceProvinces);
		sourceProvinces.clear();
	}

	// Now output information on which provinces remain unmapped
	log("Mapper::MapProvinces finished mapping provinces.\n");

	int totalUnmatched = 0;
	std::vector<Province*> provinces = destWorld.GetAllProvinces();

	bool first = true;
	for (unsigned int i = 0; i < provinces.size(); i++)
	{
		if (provinces[i]->GetSourceProvinces().size() < 1)
		{	 
			if (first)
			{
				first = false;
				log("Mapper::MapProvinces found destination province(s) with no source:\n");
			}
			log("%s\n", provinces[i]->GetName().c_str());
			totalUnmatched++;
		}
	}
	if (totalUnmatched > 0)
	{
		log("Warning: Mapper::MapProvinces found %d unmatched provinces.\n", totalUnmatched);
		printf("Warning: Mapper::MapProvinces found %d unmatched provinces.\n", totalUnmatched);
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
		{
			log("Warning: Destination country is null.\n");
			printf("Warning: Destination country is null.\n");
			continue;
		}

		for (setIter = (*mapIter).second.begin(); setIter != (*mapIter).second.end(); setIter++)
		{
			Country* orig = origWorld.GetCountry(*setIter);

			if (NULL == orig)
			{
				log("Warning: Origination country is null.\n");
				printf("Warning: Origination country is null.\n");
				continue;
			}

			sources.push_back(orig);
		}

		dest->SetSourceCountries(sources);
	}

	// Now output information on which countries remain unmapped
	log("Mapper::MapCountries finished mapping countries.\n");

	int totalUnmatched = 0;
	std::vector<Country*> countries = origWorld.GetAllCountries();

	bool first = true;
	for (unsigned int i = 0; i < countries.size(); i++)
	{
		if (countries[i]->GetDestCountry() == NULL)
		{
			if (first)
			{
				first = false;
				log("Mapper::MapCountries found countries with no destination:\n");
			}
			log("%s\n", countries[i]->GetName().c_str());
			totalUnmatched++; 
		}
	}

	if (totalUnmatched > 0)
	{
		log("Warning: Mapper::MapCountries found %d unmatched countries.\n", totalUnmatched);
		printf("Warning: Mapper::MapCountries found %d unmatched countries.\n", totalUnmatched);
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
			std::string ownerStr, coreStr;
			std::vector<Object*> oldOwnerVal, newOwnerVal, oldCoreVal, provinceData;
			std::set<std::string> cores;
			Country  *origCountry, *destCountry;	

			oldOwnerVal = sourceProvinces[0]->GetSource()->getValue("owner");	 
			if (oldOwnerVal.size() < 1)
				continue;

			ownerStr = oldOwnerVal[0]->getLeaf();
			ownerStr = ownerStr.substr(1, 3); // This was stored with quotes around it
			origCountry = origWorld.GetCountry(ownerStr);

			if (origCountry != NULL)
			{
				destCountry = origCountry->GetDestCountry();

				if (destCountry != NULL)
				{
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

			// So, who had cores on this province?
			oldCoreVal = sourceProvinces[0]->GetSource()->getValue("core");

			for (unsigned int j = 0; j < oldCoreVal.size(); j++)
			{
				coreStr = oldCoreVal[j]->getLeaf();
				coreStr = coreStr.substr(1, 3); // This was stored with quotes around it	

				origCountry = origWorld.GetCountry(coreStr);

				if (origCountry != NULL)
				{
					destCountry = origCountry->GetDestCountry();
					if (destCountry != NULL)
					{
						coreStr = "\"" + destCountry->GetName() + "\"";
						cores.insert(coreStr);
					}
				}
			}

			// We have converted the list of old countries with a core on the province to new world
			// Now remove cores and insert them
			provinceData = allProvinces[i]->GetSource()->getLeaves();
/*
			for (unsigned int j = 0; j < provinceData.size(); j++)
			{
				if (provinceData[j]->getKey().compare("core") == 0)
				{
					allProvinces[i]->GetSource()->removeObject(provinceData[j]);
				}		 
			}
*/
			for (std::set<std::string>::iterator setIter = cores.begin(); setIter != cores.end(); setIter++)
			{
				// Insert the 'core' value just before 'garrison'
				Object* newCore = new Object("core");
				newCore->setValue(*setIter);
				allProvinces[i]->GetSource()->addObjectAfter(newCore, "garrison");
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


void Mapper::AssignCountryCapitals(World& origWorld, World& destWorld)
{
	std::vector<Object*> ownerVal;
	std::vector<Country*> allCountries = destWorld.GetAllCountries();

	for (unsigned int i = 0; i < allCountries.size(); i++)
	{
		bool capitalFound = false;
		Country* country = allCountries[i];
		const std::vector<Province*> myProvinces = country->GetProvinces();

		if (myProvinces.size() < 1)
		{
			// Ignore non-existent countries - doesn't matter where their capital is
			continue;
		}

		std::string capitalLocation = country->GetSource()->getLeaf("capital");

		if (atoi(capitalLocation.c_str()) < 1)
		{
			// Rebels have this - ignore
			continue;
		}

		for (unsigned int j = 0; j < myProvinces.size(); j++)
		{
			if (myProvinces[j]->GetName().compare(capitalLocation) == 0)
			{
				capitalFound = true;
				break;
			}
		}

		if (!capitalFound)
		{
			// HACK: Set capital to be first province found 
			country->GetSource()->setLeaf("capital", myProvinces[0]->GetName());
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
			std::ostringstream stateID;
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

			stateID << "" << startState;
			CreateState(stateID.str(), country, state);
			startState++;
		}
	}
}


void Mapper::CreateState(std::string stateID, Country* country, std::vector<std::string>& provinces)
{
	std::vector<Object*> stateVal, stateIDVal;

	Object* state = new Object("state");
	Object* stateIDObj = new Object("id");
	Object* stateProvinces = new Object("provinces");	

	stateVal.push_back(stateIDObj);
	stateVal.push_back(stateProvinces);

	state->setValue(stateVal);

	// Set id tag
	Object* stateID_ID = new Object("id");
	Object* stateID_Type = new Object("type");

	stateID_ID->setValue(stateID);
	stateID_Type->setValue("47");

	stateIDVal.push_back(stateID_ID);
	stateIDVal.push_back(stateID_Type);

	stateIDObj->setValue(stateIDVal);

	// Set provinces flag
	for (unsigned int i = 0; i < provinces.size(); i++)
	{		
		stateProvinces->addToList(provinces[i]);
	}

	country->GetSource()->addObject(state);
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


std::map<std::string, std::set<std::string> > Mapper::InitEUToVickyCountryMap(Object* obj)
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
		std::set<std::string> euIDs;
		std::string vickyID;

		std::vector<Object*> euMaps = data[i]->getLeaves();

		for (unsigned int j = 0; j < euMaps.size(); j++)
		{
			if (euMaps[j]->getKey().compare("eu3") == 0)
			{		 
				euIDs.insert(euMaps[j]->getLeaf());
			}
			else if (euMaps[j]->getKey().compare("vic") == 0)
			{
				vickyID = euMaps[j]->getLeaf();
			}
			else
			{
				// Error
			}
		}

		mapping.insert(std::make_pair<std::string, std::set<std::string> >(vickyID, euIDs));
	}

	return mapping;
}