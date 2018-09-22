/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "Names.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"



class culture: commonItems::parser
{
	public:
		culture(std::istream& theStream);

		void convertNamesToUTF8();

		auto getMaleNames() const { return maleNames; }
		auto getSurnames() const { return surnames; }
		auto getFemaleNames() const { return femaleNames; }
		auto getFemaleSurnames() const { return femaleSurnames; }
		auto getCallsigns() const { return  callsigns; }
		auto getIndustryCompanies() const { return industryCompanies; }
		auto getElectronicCompanies() const { return electronicCompanies; }
		auto getCarCompanies() const { return carCompanies; }
		auto getNavalCompanies() const { return navalCompanies; }
		auto getWeaponCompanies() const { return weaponCompanies; }
		auto getAircraftCompanies() const { return aircraftCompanies; }

	private:
		std::vector<std::string> maleNames;
		std::vector<std::string> surnames;
		std::vector<std::string> femaleNames;
		std::vector<std::string> femaleSurnames;
		std::vector<std::string> callsigns;
		std::vector<std::string> industryCompanies;
		std::vector<std::string> electronicCompanies;
		std::vector<std::string> carCompanies;
		std::vector<std::string> navalCompanies;
		std::vector<std::string> weaponCompanies;
		std::vector<std::string> aircraftCompanies;
};


culture::culture(std::istream& theStream)
{
	registerKeyword(std::regex("first_names"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		maleNames = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("last_names"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		surnames = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("female_names"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		femaleNames = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("female_surnames"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		femaleSurnames = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("callsigns"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		callsigns = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("industry_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		industryCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("electronic_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		electronicCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("car_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		carCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("naval_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		navalCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("weapon_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		weaponCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("aircraft_companies"), [this](const std::string unused, std::istream& theStream){
		commonItems::stringList firstNameStrings(theStream);
		aircraftCompanies = firstNameStrings.getStrings();
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


void::culture::convertNamesToUTF8()
{
	std::for_each(maleNames.begin(), maleNames.end(), [](std::string& name){ name = Utils::convertWin1252ToUTF8(name); });
	std::for_each(surnames.begin(), surnames.end(), [](std::string& name){ name = Utils::convertWin1252ToUTF8(name); });
}


class cultureGroup: commonItems::parser
{
	public:
		cultureGroup(std::istream& theStream);

		auto getMaleNames() const { return maleNamesMap; }
		auto getSurnames() const { return surnamesMap; }

	private:
		std::map<std::string, std::vector<std::string>> maleNamesMap;
		std::map<std::string, std::vector<std::string>> surnamesMap;
};


cultureGroup::cultureGroup(std::istream& theStream)
{
	registerKeyword(std::regex("leader"), commonItems::ignoreItem);
	registerKeyword(std::regex("unit"), commonItems::ignoreItem);
	registerKeyword(std::regex("union"), commonItems::ignoreItem);
	registerKeyword(std::regex("is_overseas"), commonItems::ignoreItem);
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), [this](const std::string& cultureName, std::istream& theStream){
		culture newCulture(theStream);
		newCulture.convertNamesToUTF8();
		maleNamesMap.insert(make_pair(cultureName, newCulture.getMaleNames()));
		surnamesMap.insert(make_pair(cultureName, newCulture.getSurnames()));
	});

	parseStream(theStream);
}


HoI4::namesMapper::namesMapper() noexcept:
	rng()
{
	LOG(LogLevel::Info) << "Parsing names";

	for (auto mod: theConfiguration.getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures from " << mod;
		processVic2CulturesFile((theConfiguration.getVic2Path() + "/mod/" + mod + "/common/cultures.txt"));
	}

	processVic2CulturesFile((theConfiguration.getVic2Path() + "/common/cultures.txt"));

	processNamesFile();
	if (theConfiguration.getDebug())
	{
		checkForNames();
	}
}


void HoI4::namesMapper::processVic2CulturesFile(const std::string& filename)
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), [this](const std::string& cultureGroupName, std::istream& theStream){
		cultureGroup theGroup(theStream);
		for (auto newMaleNameMapping: theGroup.getMaleNames())
		{
			addNamesToMap(maleNamesMap, newMaleNameMapping.first, newMaleNameMapping.second);
		}
		for (auto newSurnameMapping: theGroup.getSurnames())
		{
			addNamesToMap(surnamesMap, newSurnameMapping.first, newSurnameMapping.second);
		}
	});

	parseFile(filename);
}


void HoI4::namesMapper::processNamesFile()
{
	clearRegisteredKeywords();
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), [this](const std::string& cultureName, std::istream& theStream){
		culture newCulture(theStream);
		auto tempNames = newCulture.getMaleNames();
		addNamesToMap(maleNamesMap, cultureName, tempNames);
		tempNames = newCulture.getFemaleNames();
		addNamesToMap(femaleNamesMap, cultureName, tempNames);
		tempNames = newCulture.getSurnames();
		addNamesToMap(surnamesMap, cultureName, tempNames);
		tempNames = newCulture.getFemaleSurnames();
		addNamesToMap(femaleSurnamesMap, cultureName, tempNames);
		tempNames = newCulture.getCallsigns();
		addNamesToMap(callsignsMap, cultureName, tempNames);
		tempNames = newCulture.getCarCompanies();
		addNamesToMap(carCompanyNames, cultureName, tempNames);
		tempNames = newCulture.getWeaponCompanies();
		addNamesToMap(weaponCompanyNames, cultureName, tempNames);
		tempNames = newCulture.getAircraftCompanies();
		addNamesToMap(aircraftCompanyNames, cultureName, tempNames);
		tempNames = newCulture.getNavalCompanies();
		addNamesToMap(navalCompanyNames, cultureName, tempNames);
		tempNames = newCulture.getIndustryCompanies();
		addNamesToMap(industryCompanyNames, cultureName, tempNames);
		tempNames = newCulture.getElectronicCompanies();
		addNamesToMap(electronicCompanyNames, cultureName, tempNames);
	});

	parseFile("names.txt");
}


void HoI4::namesMapper::addNamesToMap(std::map<std::string, std::vector<std::string>>& map, const std::string& culture, std::vector<std::string>& newNames)
{
	if (auto mapping = map.find(culture); mapping != map.end())
	{
		for (auto newName: newNames)
		{
			if (std::none_of(mapping->second.begin(), mapping->second.end(), [newName](std::string oldName){ return oldName == newName; }))
			{
				mapping->second.push_back(newName);
			}
		}
	}
	else
	{
		map.insert(make_pair(culture, newNames));
	}
}


void HoI4::namesMapper::checkForNames()
{
	for (auto maleNamesMapping: maleNamesMap)
	{
		auto culture = maleNamesMapping.first;

		if (femaleNamesMap.find(culture) == femaleNamesMap.end())
		{
			LOG(LogLevel::Warning) << "No female names for " << culture;
		}
		// female surnames being missing is a common and acceptable case
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


std::optional<std::vector<std::string>> HoI4::namesMapper::getMaleNames(const std::string& culture) const
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


std::optional<std::vector<std::string>> HoI4::namesMapper::getFemaleNames(const std::string& culture) const
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


std::optional<std::vector<std::string>> HoI4::namesMapper::getSurnames(const std::string& culture) const
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


std::optional<std::vector<std::string>> HoI4::namesMapper::getFemaleSurnames(const std::string& culture) const
{
	auto namesItr = femaleSurnamesMap.find(culture);
	if (namesItr != femaleSurnamesMap.end())
	{
		return namesItr->second;
	}
	else
	{
		return {};
	}
}


std::optional<std::vector<std::string>> HoI4::namesMapper::getCallsigns(const std::string& culture) const
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


std::optional<std::string> HoI4::namesMapper::getMaleName(const std::string& culture)
{
	auto firstNames = getMaleNames(culture);
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


std::optional<std::string> HoI4::namesMapper::getFemaleName(const std::string& culture)
{
	auto firstNames = getFemaleNames(culture);
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


std::optional<std::string> HoI4::namesMapper::getSurname(const std::string& culture)
{
	auto surnames = getSurnames(culture);
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


std::optional<std::string> HoI4::namesMapper::getFemaleSurname(const std::string& culture)
{
	auto surnames = getFemaleSurnames(culture);
	if (surnames)
	{
		std::uniform_int_distribution<int> surnameGen(0, surnames->size() - 1);
		return (*surnames)[surnameGen(rng)];
	}
	else
	{
		LOG(LogLevel::Warning) << "No female surname could be found for " << culture;
		return {};
	}
}


std::optional<std::string> HoI4::namesMapper::getCallsign(const std::string& culture)
{
	auto callsigns = getCallsigns(culture);
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


std::optional<std::string> HoI4::namesMapper::takeCarCompanyName(const std::string& culture)
{
	return takeCompanyName(carCompanyNames, culture);
}


std::optional<std::string> HoI4::namesMapper::takeWeaponCompanyName(const std::string& culture)
{
	return takeCompanyName(weaponCompanyNames, culture);
}


std::optional<std::string> HoI4::namesMapper::takeAircraftCompanyName(const std::string& culture)
{
	return takeCompanyName(aircraftCompanyNames,culture);
}


std::optional<std::string> HoI4::namesMapper::takeNavalCompanyName(const std::string& culture)
{
	return takeCompanyName(navalCompanyNames, culture);
}


std::optional<std::string> HoI4::namesMapper::takeIndustryCompanyName(const std::string& culture)
{
	return takeCompanyName(industryCompanyNames, culture);
}


std::optional<std::string> HoI4::namesMapper::takeElectronicCompanyName(const std::string& culture)
{
	return takeCompanyName(electronicCompanyNames, culture);
}


std::optional<std::string> HoI4::namesMapper::takeCompanyName(std::map<std::string, std::vector<std::string>>& companyNames, const std::string& culture)
{
	auto namesItr = companyNames.find(culture);
	if (namesItr != companyNames.end())
	{
		std::vector<std::string> companies = namesItr->second;
		if (companies.size() > 0)
		{
			std::uniform_int_distribution<int> surnameGen(0, companies.size() - 1);
			auto company = companies[surnameGen(rng)];
			for (std::vector<std::string>::iterator itr = companyNames[culture].begin(); itr != companyNames[culture].end(); ++itr)
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