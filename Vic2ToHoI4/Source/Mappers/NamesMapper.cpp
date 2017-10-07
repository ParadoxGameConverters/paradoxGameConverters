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
	shared_ptr<Object> obj = parser_8859_15::doParseFile(filename);
	if (obj == nullptr)
	{
		return;
	}

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


void namesMapper::processNamesFile()
{
	shared_ptr<Object> obj = parser_UTF8::doParseFile("names.txt");
	for (auto cultureObj: obj->getLeaves())
	{
		string culture = cultureObj->getKey();

		auto femaleNamesObj = cultureObj->safeGetObject("female_names");
		if (femaleNamesObj != nullptr)
		{
			femaleNamesMap.insert(make_pair(culture, femaleNamesObj->getTokens()));
		}

		auto callsignsObj = cultureObj->safeGetObject("callsigns");
		if (callsignsObj != nullptr)
		{
			callsignsMap.insert(make_pair(culture, callsignsObj->getTokens()));
		}

		auto carCompaniesObj = cultureObj->safeGetObject("car_companies");
		if (carCompaniesObj != nullptr)
		{
			carCompanyNames.insert(make_pair(culture, carCompaniesObj->getTokens()));
		}

		auto weaponCompanyObj = cultureObj->safeGetObject("weapon_companies");
		if (weaponCompanyObj != nullptr)
		{
			weaponCompanyNames.insert(make_pair(culture, weaponCompanyObj->getTokens()));
		}

		auto aircraftCompanyNamesObj = cultureObj->safeGetObject("aircraft_companies");
		if (aircraftCompanyNamesObj != nullptr)
		{
			aircraftCompanyNames.insert(make_pair(culture, aircraftCompanyNamesObj->getTokens()));
		}

		auto navalCompanyNamesObj = cultureObj->safeGetObject("naval_companies");
		if (navalCompanyNamesObj != nullptr)
		{
			navalCompanyNames.insert(make_pair(culture, navalCompanyNamesObj->getTokens()));
		}

		auto industryCompanyNamesObj = cultureObj->safeGetObject("industry_companies");
		if (industryCompanyNamesObj != nullptr)
		{
			industryCompanyNames.insert(make_pair(culture, industryCompanyNamesObj->getTokens()));
		}

		auto electronicCompanyNamesObj = cultureObj->safeGetObject("electronic_companies");
		if (electronicCompanyNamesObj != nullptr)
		{
			electronicCompanyNames.insert(make_pair(culture, electronicCompanyNamesObj->getTokens()));
		}
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


vector<string> namesMapper::GetMaleNames(const string& culture) const
{
	vector<string> maleNames;

	auto namesItr = maleNamesMap.find(culture);
	if (namesItr != maleNamesMap.end())
	{
		maleNames = namesItr->second;
	}
	else
	{
		maleNames.push_back("null");
	}

	return maleNames;
}


vector<string> namesMapper::GetFemaleNames(const string& culture) const
{
	vector<string> femaleNames;

	auto namesItr = femaleNamesMap.find(culture);
	if (namesItr != femaleNamesMap.end())
	{
		femaleNames = namesItr->second;
	}
	else
	{
		femaleNames.push_back("null");
	}

	return femaleNames;
}


vector<string> namesMapper::GetSurnames(const string& culture) const
{
	vector<string> surnames;

	auto namesItr = surnamesMap.find(culture);
	if (namesItr != surnamesMap.end())
	{
		surnames = namesItr->second;
	}
	else
	{
		surnames.push_back("null");
	}

	return surnames;
}


vector<string> namesMapper::GetCallsigns(const string& culture) const
{
	vector<string> callsigns;

	auto namesItr = callsignsMap.find(culture);
	if (namesItr != callsignsMap.end())
	{
		callsigns = namesItr->second;
	}
	else
	{
		callsigns.push_back("null");
	}

	return callsigns;
}


string namesMapper::GetMaleName(const string& culture)
{
	vector<string> firstNames = GetMaleNames(culture);

	std::uniform_int_distribution<int> firstNameGen(0, firstNames.size() - 1);
	return firstNames[firstNameGen(rng)];
}


string namesMapper::GetFemaleName(const string& culture)
{
	vector<string> firstNames = GetFemaleNames(culture);

	std::uniform_int_distribution<int> firstNameGen(0, firstNames.size() - 1);
	return firstNames[firstNameGen(rng)];
}


string namesMapper::GetSurname(const string& culture)
{
	vector<string> surnames = GetSurnames(culture);

	std::uniform_int_distribution<int> surnameGen(0, surnames.size() - 1);
	return surnames[surnameGen(rng)];
}


string namesMapper::GetCallsign(const string& culture)
{
	vector<string> callsigns = GetCallsigns(culture);

	std::uniform_int_distribution<int> surnameGen(0, callsigns.size() - 1);
	return callsigns[surnameGen(rng)];
}


string namesMapper::GetCarCompanyName(const string& culture)
{
	return getCompanyName(carCompanyNames, culture);
}


string namesMapper::GetWeaponCompanyName(const string& culture)
{
	return getCompanyName(weaponCompanyNames, culture);
}


string namesMapper::GetAircraftCompanyName(const string& culture)
{
	return getCompanyName(aircraftCompanyNames,culture);
}


string namesMapper::GetNavalCompanyName(const string& culture)
{
	return getCompanyName(navalCompanyNames, culture);
}


string namesMapper::GetIndustryCompanyName(const string& culture)
{
	return getCompanyName(industryCompanyNames, culture);
}


string namesMapper::GetElectronicCompanyName(const string& culture)
{
	return getCompanyName(electronicCompanyNames, culture);
}


string namesMapper::getCompanyName(map<string, vector<string>>& companyNames, const string& culture)
{
	string company = "";

	auto namesItr = companyNames.find(culture);
	if (namesItr != companyNames.end())
	{
		vector<string> companies = namesItr->second;
		if (companies.size() > 0)
		{
			std::uniform_int_distribution<int> surnameGen(0, companies.size() - 1);
			company = companies[surnameGen(rng)];
			for (vector<string>::iterator itr = companyNames[culture].begin(); itr != companyNames[culture].end(); ++itr)
			{
				if (*itr == company)
				{
					companyNames[culture].erase(itr);
					break;
				}
			}
		}
	}

	return company;
}