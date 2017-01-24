/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "../Mappers/CountryMapping.h"
#include "../Mappers/ProvinceDefinitions.h"
#include "../Mappers/V2Localisations.h"
#include "../V2World/V2Country.h"
#include "../V2World/V2World.h"
#include <fstream>



void HoI4States::importStates(map<int, vector<int>>& defaultStateToProvinceMap)
{
	LOG(LogLevel::Info) << "Importing states";

	set<string> statesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", statesFiles);
	for (auto stateFile: statesFiles)
	{
		int num = stoi(stateFile.substr(0, stateFile.find_first_of('-')));

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


void HoI4States::convertStates()
{
	LOG(LogLevel::Info) << "Converting states";
	map<int, ownersAndCores> provinceOwnersandCores = determineProvinceOwners();
	createStates(provinceOwnersandCores);
}


map<int, ownersAndCores> HoI4States::determineProvinceOwners()
{
	const map<string, V2Country*> V2Countries = sourceWorld->getCountries();
	map<int, ownersAndCores> ownersAndCoresMap;
	for (auto provItr: provinceDefinitions::getLandProvinces())
	{
		HoI4ToVic2ProvinceMapping::const_iterator provinceLink;
		if (!getAppropriateMapping(provItr, provinceLink))
		{
			continue;
		}

		map<const V2Country*, MTo1ProvinceComp> potentialOwners = determinePotentialOwners(provinceLink);
		const V2Country* oldOwner = selectProvinceOwner(potentialOwners);
		if (oldOwner == NULL)
		{
			continue;
		}

		// convert from the source provinces
		const string HoI4Tag = CountryMapper::getHoI4Tag(oldOwner->getTag());
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
					for (auto oldCore: srcProvItr->getCores())
					{
						// skip this core if the country is the owner of the V2 province but not the HoI4 province
						// (i.e. "avoid boundary conflicts that didn't exist in V2").
						// this country may still get core via a province that DID belong to the current HoI4 owner
						if ((oldCore == srcOwnerItr.first) && (oldCore != oldOwner))
						{
							continue;
						}

						const string coreOwner = CountryMapper::getHoI4Tag(oldCore->getTag());
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


bool HoI4States::getAppropriateMapping(int provNum, HoI4ToVic2ProvinceMapping::const_iterator& provinceLink)
{
	provinceLink = provinceMapper::getHoI4ToVic2ProvinceMapping().find(provNum);
	if ((provinceLink == provinceMapper::getHoI4ToVic2ProvinceMapping().end()) || (provinceLink->second.size() == 0))
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


map<const V2Country*, MTo1ProvinceComp> HoI4States::determinePotentialOwners(HoI4ToVic2ProvinceMapping::const_iterator provinceLink)
{
	map<const V2Country*, MTo1ProvinceComp> potentialOwners;
	for (auto srcProvItr: provinceLink->second)
	{
		auto srcProvince = sourceWorld->getProvince(srcProvItr);
		if (!srcProvince)
		{
			LOG(LogLevel::Warning) << "Old province " << provinceLink->second[0] << " does not exist (bad mapping?)";
			continue;
		}
		const V2Country* owner = srcProvince->getOwner();

		if (potentialOwners.find(owner) == potentialOwners.end())
		{
			potentialOwners[owner] = MTo1ProvinceComp();
		}
		potentialOwners[owner].provinces.push_back(srcProvince);
		potentialOwners[owner].totalPopulation += srcProvince->getTotalPopulation();
	}

	return potentialOwners;
}


const V2Country* HoI4States::selectProvinceOwner(const map<const V2Country*, MTo1ProvinceComp>& potentialOwners)
{
	const V2Country* oldOwner = NULL;
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


void HoI4States::createStates(const map<int, ownersAndCores>& provinceToOwnersAndCoresMap)
{
	int stateID = 1;
	set<int> assignedProvinces;
	for (auto country: sourceWorld->getCountries())
	{
		for (auto vic2State: country.second->getStates())
		{		
			if (createMatchingHoI4State(vic2State, stateID, CountryMapper::getHoI4Tag(country.first), provinceToOwnersAndCoresMap, assignedProvinces))
			{
				stateID++;
			}
		}
	}

	unsigned int manpower = getTotalManpower();
	LOG(LogLevel::Debug) << "Total manpower was " << manpower << ", which is " << manpower / 20438756.2 << "% of default HoI4.";
}


bool HoI4States::createMatchingHoI4State(const Vic2State* vic2State, int stateID, const string& stateOwner, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces)
{
	//	create a matching HoI4 state
	HoI4State* newState = new HoI4State(vic2State, stateID, stateOwner);
	addProvincesToNewState(newState, provinceToOwnersAndCoresMap, assignedProvinces);
	if (newState->getProvinces().size() == 0)
	{
		delete newState;
		return false;
	}

	createVPForState(newState);
	addManpowerToNewState(newState);
	states.insert(make_pair(stateID, newState));

	return true;
}


void HoI4States::addProvincesToNewState(HoI4State* newState, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces)
{
	for (auto vic2ProvinceNum: newState->getSourceState()->getProvinceNums())
	{
		//	if the matching HoI4 provinces are owned by this country, add it to the HoI4 state
		auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(vic2ProvinceNum);
		if (provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
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


void HoI4States::createVPForState(HoI4State* newState)
{
	if (!newState->tryToCreateVP())
	{
		LOG(LogLevel::Warning) << "Could not create VP for state";
	}
}


void HoI4States::addManpowerToNewState(HoI4State* newState)
{
	for (auto sourceProvince: newState->getSourceState()->getProvinces())
	{
		int additionalManpower = static_cast<int>(sourceProvince->getTotalPopulation() * 4 * Configuration::getManpowerFactor());
		newState->addManpower(additionalManpower);
	}
}


void HoI4States::addLocalisations()
{
	for (auto state: states)
	{
		for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage(state.second->getSourceState()->getStateID()))
		{
			addStateLocalisationForLanguage(state.second, Vic2NameInLanguage);
		}

		int VPPositionInHoI4 = state.second->getVPLocation();
		auto VPProvinceMapping = provinceMapper::getHoI4ToVic2ProvinceMapping().find(VPPositionInHoI4);
		if (
			  (VPProvinceMapping != provinceMapper::getHoI4ToVic2ProvinceMapping().end()) &&
			  (VPProvinceMapping->second.size() > 0)
			)
		{
			for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage("PROV" + to_string(VPProvinceMapping->second[0])))
			{
				addVPLocalisationForLanguage(state.second, Vic2NameInLanguage);
			}
		}
	}

	addNonenglishStateLocalisations();
	addNonenglishVPLocalisations();
}


void HoI4States::addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage)
{
	getExistingStateLocalisation(Vic2NameInLanguage.first).insert(state->makeLocalisation(Vic2NameInLanguage));
}


void HoI4States::addVPLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage)
{
	getExistingVPLocalisation(Vic2NameInLanguage.first).insert(state->makeVPLocalisation(Vic2NameInLanguage));
}


keyToLocalisationMap& HoI4States::getExistingStateLocalisation(const string& language)
{
	auto existingLocalisation = stateLocalisations.find(language);
	if (existingLocalisation == stateLocalisations.end())
	{
		addLanguageToStateLocalisations(language);
		existingLocalisation = stateLocalisations.find(language);
	}

	return existingLocalisation->second;
}


keyToLocalisationMap& HoI4States::getExistingVPLocalisation(const string& language)
{
	auto existingLocalisation = VPLocalisations.find(language);
	if (existingLocalisation == VPLocalisations.end())
	{
		addLanguageToVPLocalisations(language);
		existingLocalisation = VPLocalisations.find(language);
	}

	return existingLocalisation->second;
}


void HoI4States::addLanguageToStateLocalisations(const string& language)
{
	keyToLocalisationMap newLocalisation;
	stateLocalisations[language] = newLocalisation;
}


void HoI4States::addLanguageToVPLocalisations(const string& language)
{
	keyToLocalisationMap newLocalisation;
	VPLocalisations[language] = newLocalisation;
}


void HoI4States::addNonenglishStateLocalisations()
{
	auto englishLocalisations = stateLocalisations.find("english");
	stateLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	stateLocalisations.insert(make_pair("polish", englishLocalisations->second));
	stateLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


void HoI4States::addNonenglishVPLocalisations()
{
	auto englishLocalisations = VPLocalisations.find("english");
	VPLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	VPLocalisations.insert(make_pair("polish", englishLocalisations->second));
	VPLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


unsigned int HoI4States::getTotalManpower()
{
	unsigned int totalManpower = 0;
	for (auto state: states)
	{
		totalManpower += state.second->getManpower();
	}

	return totalManpower;
}


void HoI4States::output() const
{
	LOG(LogLevel::Debug) << "Writing states";
	outputHistory();
	outputStateLocalisations();
	outputVPLocalisations();
}


void HoI4States::outputHistory() const
{
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/history"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history";
		exit(-1);
	}
	if (!Utils::TryCreateFolder("Output/" + Configuration::getOutputName() + "/history/states"))
	{
		LOG(LogLevel::Error) << "Could not create \"Output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	for (auto state: states)
	{
		state.second->output(to_string(state.first) + ".txt");
	}
}


void HoI4States::outputStateLocalisations() const
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
		localisationFile << "l_" << languageToLocalisations.first << ":\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":10 \"" << mapping.second << "\"" << endl;
		}
	}
}


void HoI4States::outputVPLocalisations() const
{
	for (auto languageToLocalisations: VPLocalisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		ofstream localisationFile("Output/" + Configuration::getOutputName() + "/localisation/victory_points_l_" + languageToLocalisations.first + ".yml");
		if (!localisationFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not update localisation text file";
			exit(-1);
		}
		localisationFile << "\xEF\xBB\xBF"; // output a BOM to make HoI4 happy
		localisationFile << "l_" << languageToLocalisations.first << ":\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":10 \"" << mapping.second << "\"" << endl;
		}
	}
}