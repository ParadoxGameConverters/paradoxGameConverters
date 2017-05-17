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
#include "HoI4State.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "../Mappers/CountryMapping.h"
#include "../Mappers/ProvinceDefinitions.h"
#include "../Mappers/V2Localisations.h"
#include "../V2World/V2Country.h"
#include "../V2World/V2Province.h"
#include "../V2World/Vic2State.h"
#include "../V2World/V2World.h"
#include <fstream>



HoI4States::HoI4States(const V2World* _sourceWorld)
{
	LOG(LogLevel::Info) << "Converting states";

	sourceWorld = _sourceWorld;
	determineOwnersAndCores();
	createStates();
}


void HoI4States::determineOwnersAndCores()
{
	for (auto provinceNumber: provinceDefinitions::getLandProvinces())
	{
		auto sourceProvinceNums = retrieveSourceProvinceNums(provinceNumber);

		map<const V2Country*, pair<int, int>> potentialOwners = determinePotentialOwners(sourceProvinceNums);

		const V2Country* oldOwner = selectProvinceOwner(potentialOwners);
		if (oldOwner == nullptr)
		{
			continue;
		}

		const string HoI4Tag = CountryMapper::getHoI4Tag(oldOwner->getTag());
		if (HoI4Tag.empty())
		{
			LOG(LogLevel::Warning) << "Could not map states owned by " << oldOwner->getTag() << " in Vic2";
			continue;
		}
		ownersMap.insert(make_pair(provinceNumber, HoI4Tag));

		vector<string> cores = determineCores(sourceProvinceNums, oldOwner);
		coresMap.insert(make_pair(provinceNumber, cores));
	}
}


vector<int> HoI4States::retrieveSourceProvinceNums(int provNum) const
{
	vector<int> none;

	auto provinceLink = provinceMapper::getHoI4ToVic2ProvinceMapping().find(provNum);
	if ((provinceLink == provinceMapper::getHoI4ToVic2ProvinceMapping().end()) || (provinceLink->second.size() == 0))
	{
		LOG(LogLevel::Warning) << "No source for HoI4 land province " << provNum;
		return none;
	}
	else if (provinceLink->second[0] == 0)
	{
		return none;
	}
	else
	{
		return provinceLink->second;
	}
}


map<const V2Country*, pair<int, int>> HoI4States::determinePotentialOwners(const vector<int>& sourceProvinceNums) const
{
	map<const V2Country*, pair<int, int>> potentialOwners;

	for (auto srcProvNum: sourceProvinceNums)
	{
		auto srcProvince = sourceWorld->getProvince(srcProvNum);
		if (srcProvince == nullptr)
		{
			LOG(LogLevel::Warning) << "Old province " << srcProvNum << " does not exist (bad mapping?)";
			continue;
		}
		const V2Country* owner = srcProvince->getOwner();

		if (potentialOwners.find(owner) == potentialOwners.end())
		{
			potentialOwners[owner] = make_pair(0, 0);
		}
		potentialOwners[owner].first++;
		potentialOwners[owner].second += srcProvince->getTotalPopulation();
	}

	return potentialOwners;
}


const V2Country* HoI4States::selectProvinceOwner(const map<const V2Country*, pair<int, int>>& potentialOwners) const
{
	const V2Country* oldOwner = nullptr;
	for (auto potentialOwner: potentialOwners)
	{
		// I am the new owner if there is no current owner, or I have more provinces than the current owner,
		// or I have the same number of provinces, but more population, than the current owner
		if (	(oldOwner == nullptr) ||
				(potentialOwner.second.first > potentialOwners.find(oldOwner)->second.first) ||
				(
					(potentialOwner.second.first == potentialOwners.find(oldOwner)->second.first) &&
					(potentialOwner.second.second > potentialOwners.find(oldOwner)->second.second)
				)
			)
		{
			oldOwner = potentialOwner.first;
		}
	}

	return oldOwner;
}


vector<string> HoI4States::determineCores(const vector<int>& sourceProvinces, const V2Country* Vic2Owner) const
{
	vector<string> cores;

	for (auto sourceProvinceNum: sourceProvinces)
	{
		auto sourceProvince = sourceWorld->getProvince(sourceProvinceNum);
		if (sourceProvince == nullptr)
		{
			continue;
		}

		for (auto Vic2Core: sourceProvince->getCores())
		{
			// skip this core if the country is the owner of the V2 province but not the HoI4 province
			// (i.e. "avoid boundary conflicts that didn't exist in V2").
			// this country may still get core via a province that DID belong to the current HoI4 owner
			if ((Vic2Core == Vic2Owner) && (Vic2Core != Vic2Owner))
			{
				continue;
			}

			const string HoI4CoreTag = CountryMapper::getHoI4Tag(Vic2Core->getTag());
			if (HoI4CoreTag != "")
			{
				cores.push_back(HoI4CoreTag);
			}
		}
	}

	return cores;
}


void HoI4States::createStates()
{
	int stateID = 1;
	set<int> assignedProvinces;
	for (auto country: sourceWorld->getCountries())
	{
		for (auto vic2State: country.second->getStates())
		{		
			if (createMatchingHoI4State(vic2State, stateID, CountryMapper::getHoI4Tag(country.first)))
			{
				stateID++;
			}
		}
	}

	unsigned int manpower = getTotalManpower();
	LOG(LogLevel::Debug) << "Total manpower was " << manpower << ", which is " << manpower / 20438756.2 << "% of default HoI4.";
}


bool HoI4States::createMatchingHoI4State(const Vic2State* vic2State, int stateID, const string& stateOwner)
{
	HoI4State* newState = new HoI4State(vic2State, stateID, stateOwner);
	addProvincesAndCoresToNewState(newState);
	if (newState->getProvinces().size() == 0)
	{
		delete newState;
		return false;
	}

	newState->tryToCreateVP();
	newState->addManpower();
	states.insert(make_pair(stateID, newState));

	return true;
}


void HoI4States::addProvincesAndCoresToNewState(HoI4State* newState)
{
	for (auto vic2ProvinceNum: newState->getSourceState()->getProvinceNums())
	{
		auto provMapping = provinceMapper::getVic2ToHoI4ProvinceMapping().find(vic2ProvinceNum);
		if (provMapping != provinceMapper::getVic2ToHoI4ProvinceMapping().end())
		{
			for (auto HoI4ProvNum: provMapping->second)
			{
				if (
						isProvinceValid(HoI4ProvNum) &&
						isProvinceOwnedByCountry(HoI4ProvNum, newState->getOwner()) &&
						isProvinceNotAlreadyAssigned(HoI4ProvNum)
					)

				{
					newState->addProvince(HoI4ProvNum);
					provinceToStateIDMap.insert(make_pair(HoI4ProvNum, newState->getID()));
					assignedProvinces.insert(HoI4ProvNum);

					newState->addCores(coresMap.find(HoI4ProvNum)->second);
				}
			}
		}
	}
}


bool HoI4States::isProvinceValid(int provNum) const
{
	if (provNum == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool HoI4States::isProvinceOwnedByCountry(int provNum, string stateOwner) const
{
	auto provinceOwnerItr = ownersMap.find(provNum);
	if ((provinceOwnerItr == ownersMap.end()) || (provinceOwnerItr->second != stateOwner))
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool HoI4States::isProvinceNotAlreadyAssigned(int provNum) const
{
	if ((assignedProvinces.find(provNum) != assignedProvinces.end()))
	{
		return false;
	}
	else
	{
		return true;
	}
}


unsigned int HoI4States::getTotalManpower() const
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

	if (!Utils::TryCreateFolder("output/" + Configuration::getOutputName() + "/history/states"))
	{
		LOG(LogLevel::Error) << "Could not create \"output/" + Configuration::getOutputName() + "/history/states";
		exit(-1);
	}
	for (auto state: states)
	{
		state.second->output(to_string(state.first) + ".txt");
	}
}