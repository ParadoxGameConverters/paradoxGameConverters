/*Copyright (c) 2016 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "HoI4States.h"
#include "log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "..\V2World\V2Country.h"
#include "..\V2World\V2World.h"
#include <fstream>



void HoI4States::importStates(map<int, vector<int>>& defaultStateToProvinceMap)
{
	LOG(LogLevel::Info) << "Importing states";

	set<string> statesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", statesFiles);
	for (auto stateFile: statesFiles)
	{
		int num = stoi(stateFile.substr(0, stateFile.find_first_of('-')));
		stateFilenames.insert(make_pair(num, stateFile));

		// create the default state map
		Object* fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/history/states/" + stateFile);
		if (fileObj == nullptr)
		{
			LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/history/states/" << stateFile;
			exit(-1);
		}
		auto stateObj = fileObj->getValue("state");
		auto provincesObj = stateObj[0]->getValue("provinces");
		auto tokens = provincesObj[0]->getTokens();
		vector<int> provinces;
		for (auto provinceNumString: tokens)
		{
			provinces.push_back(stoi(provinceNumString));
		}

		defaultStateToProvinceMap.insert(make_pair(num, provinces));
	}
}


void HoI4States::recordAllLandProvinces()
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = atoi(line.substr(0, pos).c_str());

		line = line.substr(pos + 1, line.length());
		int pos2 = line.find_first_of(';');
		line = line.substr(pos2 + 1, line.length());
		int pos3 = line.find_first_of(';');
		line = line.substr(pos3 + 1, line.length());
		int pos4 = line.find_first_of(';');
		line = line.substr(pos4 + 1, line.length());
		int pos5 = line.find_first_of(';');
		line = line.substr(0, pos5);

		if (line == "land")
		{
			landProvinces.insert(provNum);
		}
	}
}


void HoI4States::convertStates(const HoI4ToVic2ProvinceMapping& provinceMap, const Vic2ToHoI4ProvinceMapping& inverseProvinceMap, const CountryMapping& countryMap, const V2Localisation& Vic2Localisations)
{
	map<int, ownersAndCores> provinceOwnersandCores = determineProvinceOwners(provinceMap, countryMap);
	createStates(countryMap, inverseProvinceMap, provinceOwnersandCores, Vic2Localisations);
}


map<int, ownersAndCores> HoI4States::determineProvinceOwners(const HoI4ToVic2ProvinceMapping& provinceMap, const CountryMapping& countryMap)
{
	map<int, ownersAndCores> ownersAndCoresMap;
	for (auto provItr: landProvinces)
	{
		HoI4ToVic2ProvinceMapping::const_iterator provinceLink;
		if (!getAppropriateMapping(provinceMap, provItr, provinceLink))
		{
			continue;
		}

		map<V2Country*, MTo1ProvinceComp> potentialOwners = determinePotentialOwners(provinceLink);
		V2Country* oldOwner = selectProvinceOwner(potentialOwners);
		if (oldOwner == NULL)
		{
			continue;
		}

		// convert from the source provinces
		const string HoI4Tag = countryMap[oldOwner->getTag()];
		if (HoI4Tag.empty())
		{
			LOG(LogLevel::Warning) << "Could not map provinces owned by " << oldOwner->getTag() << " in Vic2";
		}
		else
		{
			ownersAndCores newOwnerandCores;
			newOwnerandCores.owner = HoI4Tag;
			ownersAndCoresMap.insert(make_pair(provItr, newOwnerandCores));
			for (auto srcOwnerItr: potentialOwners)
			{
				for (auto srcProvItr: srcOwnerItr.second.provinces)
				{
					// convert cores
					vector<V2Country*> oldCores = srcProvItr->getCores(sourceWorld->getCountries());
					for (auto oldCoreItr: oldCores)
					{
						// skip this core if the country is the owner of the V2 province but not the HoI4 province
						// (i.e. "avoid boundary conflicts that didn't exist in V2").
						// this country may still get core via a province that DID belong to the current HoI4 owner
						if ((oldCoreItr == srcOwnerItr.first) && (oldCoreItr != oldOwner))
						{
							continue;
						}

						const string coreOwner = countryMap[oldCoreItr->getTag()];
						if (coreOwner != "")
						{
							auto coreItr = ownersAndCoresMap.find(provItr);
							if (coreItr == ownersAndCoresMap.end())
							{
								ownersAndCores newOwnerandCores;
								newOwnerandCores.cores.push_back(coreOwner);
								ownersAndCoresMap.insert(make_pair(provItr, newOwnerandCores));
							}
							else
							{
								coreItr->second.cores.push_back(coreOwner);
							}
						}
					}
				}
			}
		}
	}

	return ownersAndCoresMap;
}


bool HoI4States::getAppropriateMapping(const HoI4ToVic2ProvinceMapping& provinceMap, int provNum, HoI4ToVic2ProvinceMapping::const_iterator& provinceLink)
{
	provinceLink = provinceMap.find(provNum);
	if ((provinceLink == provinceMap.end()) || (provinceLink->second.size() == 0))
	{
		LOG(LogLevel::Warning) << "No source for HoI4 land province " << provNum;
		return false;
	}
	else if (provinceLink->second[0] == 0)
	{
		return false;
	}

	return true;
}


map<V2Country*, MTo1ProvinceComp> HoI4States::determinePotentialOwners(HoI4ToVic2ProvinceMapping::const_iterator provinceLink)
{
	map<V2Country*, MTo1ProvinceComp> potentialOwners;
	for (auto srcProvItr: provinceLink->second)
	{
		V2Province* srcProvince = sourceWorld->getProvince(srcProvItr);
		if (!srcProvince)
		{
			LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
			continue;
		}
		V2Country* owner = srcProvince->getOwner();

		if (potentialOwners.find(owner) == potentialOwners.end())
		{
			potentialOwners[owner] = MTo1ProvinceComp();
		}
		potentialOwners[owner].provinces.push_back(srcProvince);
		potentialOwners[owner].totalPopulation += srcProvince->getTotalPopulation();
	}

	return potentialOwners;
}


V2Country* HoI4States::selectProvinceOwner(const map<V2Country*, MTo1ProvinceComp>& potentialOwners)
{
	V2Country* oldOwner = NULL;
	for (auto potentialOwner: potentialOwners)
	{
		// I am the new owner if there is no current owner, or I have more provinces than the current owner,
		// or I have the same number of provinces, but more population, than the current owner
		if ((oldOwner == NULL)
			|| (potentialOwner.second.provinces.size() > potentialOwners.find(oldOwner)->second.provinces.size())
			|| ((potentialOwner.second.provinces.size() == potentialOwners.find(oldOwner)->second.provinces.size())
				&& (potentialOwner.second.totalPopulation > potentialOwners.find(oldOwner)->second.totalPopulation)))
		{
			oldOwner = potentialOwner.first;
		}
	}

	return oldOwner;
}


void HoI4States::createStates(const CountryMapping& countryMap, const Vic2ToHoI4ProvinceMapping& HoI4ToVic2ProvinceMap, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, const V2Localisation& Vic2Localisations)
{
	int stateID = 1;
	set<int> assignedProvinces;
	for (auto country: sourceWorld->getCountries())
	{
		for (auto vic2State: country.second->getStates())
		{		
			if (createMatchingHoI4State(vic2State, stateID, countryMap.GetHoI4Tag(country.first), HoI4ToVic2ProvinceMap, provinceToOwnersAndCoresMap, assignedProvinces, Vic2Localisations))
			{
				stateID++;
			}
		}
	}
}


bool HoI4States::createMatchingHoI4State(const Vic2State* vic2State, int stateID, const string& stateOwner, const Vic2ToHoI4ProvinceMapping& HoI4ToVic2ProvinceMap, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces, const V2Localisation& Vic2Localisations)
{
	//	create a matching HoI4 state
	HoI4State* newState = new HoI4State(vic2State, stateID, stateOwner);
	addProvincesToNewState(newState, HoI4ToVic2ProvinceMap, provinceToOwnersAndCoresMap, assignedProvinces);
	if (newState->getProvinces().size() == 0)
	{
		delete newState;
		return false;
	}

	createVPForState(newState, HoI4ToVic2ProvinceMap);
	addManpowerToNewState(newState);
	addLocalisation(stateID, vic2State->getStateID(), Vic2Localisations);
	states.insert(make_pair(stateID, newState));

	return true;
}


void HoI4States::addProvincesToNewState(HoI4State* newState, const Vic2ToHoI4ProvinceMapping& HoI4ToVic2ProvinceMap, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces)
{
	for (auto vic2Province: newState->getSourceState()->getProvinces())
	{
		//	if the matching HoI4 provinces are owned by this country, add it to the HoI4 state
		auto provMapping = HoI4ToVic2ProvinceMap.find(vic2Province);
		if (provMapping != HoI4ToVic2ProvinceMap.end())
		{
			for (auto HoI4ProvNum: provMapping->second)
			{
				if (isProvinceOwnedByCountryAndNotAlreadyAssigned(HoI4ProvNum, newState->getOwner(), provinceToOwnersAndCoresMap, assignedProvinces))
				{
					newState->addProvince(HoI4ProvNum);
					provinceToStateIDMap.insert(make_pair(HoI4ProvNum, newState->getID()));
					assignedProvinces.insert(HoI4ProvNum);

					newState->addCores(provinceToOwnersAndCoresMap.find(HoI4ProvNum)->second.cores);
				}
			}
		}
	}
}


bool HoI4States::isProvinceOwnedByCountryAndNotAlreadyAssigned(int provNum, string stateOwner, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces)
{
	if (provNum == 0)
	{
		return false;
	}

	auto provinceOwnerItr = provinceToOwnersAndCoresMap.find(provNum);
	if ((provinceOwnerItr == provinceToOwnersAndCoresMap.end()) || (provinceOwnerItr->second.owner != stateOwner))
	{
		return false;
	}

	if ((assignedProvinces.find(provNum) != assignedProvinces.end()))
	{
		return false;
	}

	return true;
}


void HoI4States::createVPForState(HoI4State* newState, const Vic2ToHoI4ProvinceMapping& provinceMap)
{
	int HoI4ProvNum = newState->getFirstProvinceByVic2Definition(provinceMap);
	if (newState->isProvinceInState(HoI4ProvNum))
	{
		newState->addVP(HoI4ProvNum, 5);
	}
}


void HoI4States::addManpowerToNewState(HoI4State* newState)
{
	for (auto vic2ProvNum: newState->getSourceState()->getProvinces())
	{
		V2Province* sourceProvince = sourceWorld->getProvince(vic2ProvNum);
		newState->addManpower(sourceProvince->getTotalPopulation() * 4);
	}
}


void HoI4States::addLocalisation(int HoI4StateID, string Vic2StateID, const V2Localisation& Vic2Localisations)
{
	for (auto nameInLanguage: Vic2Localisations.GetTextInEachLanguage(Vic2StateID))
	{
		auto existingLocalisation = stateLocalisations.find(nameInLanguage.first);
		if (existingLocalisation == stateLocalisations.end())
		{
			keyToLocalisationMap newLocalisation;
			stateLocalisations[nameInLanguage.first] = newLocalisation;
			existingLocalisation = stateLocalisations.find(nameInLanguage.first);
		}

		existingLocalisation->second.insert(make_pair(string("STATE_") + to_string(HoI4StateID), nameInLanguage.second));
	}
}


void HoI4States::output() const
{
	LOG(LogLevel::Debug) << "Writing states";
	outputHistory();
	outputLocalisations();
}


void HoI4States::outputHistory() const
{
	string statesPath = "Output/" + Configuration::getOutputName() + "/history/states";
	if (!Utils::TryCreateFolder(statesPath))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	for (auto state: states)
	{
		string filename;
		auto nameItr = stateFilenames.find(state.first);
		if (nameItr == stateFilenames.end())
		{
			filename = to_string(state.first) + "-convertedState.txt";
		}
		else
		{
			filename = nameItr->second;
		}
		state.second->output(filename);
	}
	for (auto nameItr = stateFilenames.find(states.size() + 1); nameItr != stateFilenames.end(); nameItr++)
	{
		ofstream emptyStateFile("Output/" + Configuration::getOutputName() + "/history/states/" + nameItr->second);
		if (!emptyStateFile.is_open())
		{
			LOG(LogLevel::Warning) << "Could not create " << "Output/" << Configuration::getOutputName() << "/history/states/" << nameItr->second;
		}
		emptyStateFile.close();
	}
}


void HoI4States::outputLocalisations() const
{
	for (auto languageToLocalisations: stateLocalisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		ofstream localisationFile("Output/" + Configuration::getOutputName() + "/localisation/state_names_l_" + languageToLocalisations.first + ".yml");
		if (!localisationFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not update localisation text file";
			exit(-1);
		}
		localisationFile << "\xEF\xBB\xBF"; // output a BOM to make HoI4 happy
		localisationFile << "l_" << languageToLocalisations.first << ":\r\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":10 \"" << mapping.second << "\"" << endl;
		}
	}
}