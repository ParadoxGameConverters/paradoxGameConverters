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



#include "NamesMapper.h"
#include "Log.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"



namesMapper* namesMapper::instance = nullptr;



namesMapper::namesMapper():
	maleNamesMap(),
	femaleNamesMap(),
	surnamesMap(),
	callsignsMap(),
	carCompanyNames(),
	weaponCompanyNames(),
	aircraftCompanyNames(),
	navalCompanyNames(),
	industryCompanyNames(),
	electronicCompanyNames(),
	rng()
{
	LOG(LogLevel::Info) << "Parsing names";

	for (auto mod: Configuration::getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures from " << mod;
		processVic2CulturesFile((Configuration::getV2Path() + "/mod/" + mod + "/common/cultures.txt"));
	}

	processVic2CulturesFile((Configuration::getV2Path() + "/common/cultures.txt"));

	processNamesFile();
	if (Configuration::getDebug())
	{
		checkForNames();
	}
}


void namesMapper::processVic2CulturesFile(const string& filename)
{
	auto obj = parser_8859_15::doParseFile(filename);
	if (obj)
	{
		vector<shared_ptr<Object>> groupsObj = obj->getLeaves();
		for (auto groupsItr: groupsObj)
		{
			vector<shared_ptr<Object>> culturesObj = groupsItr->getLeaves();
			for (auto culturesItr: culturesObj)
			{
				string key = culturesItr->getKey();
				if ((key == "union") || (key == "leader") || (key == "unit") || (key == "is_overseas"))
				{
					continue;
				}

				auto firstNamesObj = culturesItr->safeGetObject("first_names");
				auto lastNamesObj = culturesItr->safeGetObject("last_names");
				if ((firstNamesObj != nullptr) && (lastNamesObj != nullptr))
				{
					maleNamesMap.insert(make_pair(key, firstNamesObj->getTokens()));
					surnamesMap.insert(make_pair(key, lastNamesObj->getTokens()));
				}
				else
				{
					LOG(LogLevel::Error) << "No names for " << key;
				}
			}
		}
	}
	else
	{
		return;
	}
}


void namesMapper::processNamesFile()
{
	auto obj = parser_UTF8::doParseFile("names.txt");
	if (obj)
	{
		for (auto cultureObj: obj->getLeaves())
		{
			string culture = cultureObj->getKey();

			femaleNamesMap.insert(make_pair(culture, cultureObj->safeGetTokens("female_names")));
			callsignsMap.insert(make_pair(culture, cultureObj->safeGetTokens("callsigns")));
			carCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("car_companies")));
			weaponCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("weapon_companies")));
			aircraftCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("aircraft_companies")));
			navalCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("naval_companies")));
			industryCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("industry_companies")));
			electronicCompanyNames.insert(make_pair(culture, cultureObj->safeGetTokens("electronic_companies")));
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse names.txt";
		exit(-1);
	}
}


void namesMapper::checkForNames()
{
	for (auto maleNamesMapping: maleNamesMap)
	{
		auto culture = maleNamesMapping.first;

		if (femaleNamesMap.find(culture) == femaleNamesMap.end())
		{
			LOG(LogLevel::Warning) << "No female names for " << culture;
		}
		if (callsignsMap.find(culture) == callsignsMap.end())
		{
			LOG(LogLevel::Warning) << "No callsigns for " << culture;
		}
		if (carCompanyNames.find(culture) == carCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No car companies for " << culture;
		}
		if (weaponCompanyNames.find(culture) == weaponCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No weapon companies for " << culture;
		}
		if (aircraftCompanyNames.find(culture) == aircraftCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No aircraft companies for " << culture;
		}
		if (navalCompanyNames.find(culture) == navalCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No naval companies for " << culture;
		}
		if (industryCompanyNames.find(culture) == industryCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No industry companies for " << culture;
		}
		if (electronicCompanyNames.find(culture) == electronicCompanyNames.end())
		{
			LOG(LogLevel::Warning) << "No electronic companies for " << culture;
		}
	}
}


optional<vector<string>> namesMapper::GetMaleNames(const string& culture) const
{
	auto namesItr = maleNamesMap.find(culture);
	if (namesItr != maleNamesMap.end())
	{
		return namesItr->second;
	}
	else
	{
		return {};
	}
}


optional<vector<string>> namesMapper::GetFemaleNames(const string& culture) const
{
	auto namesItr = femaleNamesMap.find(culture);
	if (namesItr != femaleNamesMap.end())
	{
		return namesItr->second;
	}
	else
	{
		return {};
	}
}


optional<vector<string>> namesMapper::GetSurnames(const string& culture) const
{
	auto namesItr = surnamesMap.find(culture);
	if (namesItr != surnamesMap.end())
	{
		return namesItr->second;
	}
	else
	{
		return {};
	}
}


optional<vector<string>> namesMapper::GetCallsigns(const string& culture) const
{
	auto namesItr = callsignsMap.find(culture);
	if (namesItr != callsignsMap.end())
	{
		return namesItr->second;
	}
	else
	{
		return {};
	}
}


optional<string> namesMapper::GetMaleName(const string& culture)
{
	auto firstNames = GetMaleNames(culture);
	if (firstNames)
	{
		std::uniform_int_distribution<int> firstNameGen(0, firstNames->size() - 1);
		return (*firstNames)[firstNameGen(rng)];
	}
	else
	{
		LOG(LogLevel::Warning) << "No male name could be found for " << culture;
		return {};
	}
}


optional<string> namesMapper::GetFemaleName(const string& culture)
{
	auto firstNames = GetFemaleNames(culture);
	if (firstNames)
	{
		std::uniform_int_distribution<int> firstNameGen(0, firstNames->size() - 1);
		return (*firstNames)[firstNameGen(rng)];
	}
	else
	{
		LOG(LogLevel::Warning) << "No female name could be found for " << culture;
		return {};
	}
}


optional<string> namesMapper::GetSurname(const string& culture)
{
	auto surnames = GetSurnames(culture);
	if (surnames)
	{
		std::uniform_int_distribution<int> surnameGen(0, surnames->size() - 1);
		return (*surnames)[surnameGen(rng)];
	}
	else
	{
		LOG(LogLevel::Warning) << "No surname name could be found for " << culture;
		return {};
	}
}


optional<string> namesMapper::GetCallsign(const string& culture)
{
	auto callsigns = GetCallsigns(culture);
	if (callsigns)
	{
		std::uniform_int_distribution<int> surnameGen(0, callsigns->size() - 1);
		return (*callsigns)[surnameGen(rng)];
	}
	else
	{
		LOG(LogLevel::Warning) << "No callsign could be found for " << culture;
		return {};
	}
}


optional<string> namesMapper::GetCarCompanyName(const string& culture)
{
	return getCompanyName(carCompanyNames, culture);
}


optional<string> namesMapper::GetWeaponCompanyName(const string& culture)
{
	return getCompanyName(weaponCompanyNames, culture);
}


optional<string> namesMapper::GetAircraftCompanyName(const string& culture)
{
	return getCompanyName(aircraftCompanyNames,culture);
}


optional<string> namesMapper::GetNavalCompanyName(const string& culture)
{
	return getCompanyName(navalCompanyNames, culture);
}


optional<string> namesMapper::GetIndustryCompanyName(const string& culture)
{
	return getCompanyName(industryCompanyNames, culture);
}


optional<string> namesMapper::GetElectronicCompanyName(const string& culture)
{
	return getCompanyName(electronicCompanyNames, culture);
}


optional<string> namesMapper::getCompanyName(map<string, vector<string>>& companyNames, const string& culture)
{
	auto namesItr = companyNames.find(culture);
	if (namesItr != companyNames.end())
	{
		vector<string> companies = namesItr->second;
		if (companies.size() > 0)
		{
			std::uniform_int_distribution<int> surnameGen(0, companies.size() - 1);
			auto company = companies[surnameGen(rng)];
			for (vector<string>::iterator itr = companyNames[culture].begin(); itr != companyNames[culture].end(); ++itr)
			{
				if (*itr == company)
				{
					companyNames[culture].erase(itr);
					return company;
				}
			}
		}
	}

	return {};
}