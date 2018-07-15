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



#ifndef NAMES_MAPPER_H
#define NAMES_MAPPER_H



#include "newParser.h"
#include <optional>
#include <map>
#include <random>
#include <string>
#include <vector>



namespace HoI4
{

class namesMapper: commonItems::parser
{
	public:
		namesMapper() noexcept;
		std::optional<std::vector<std::string>> getMaleNames(const std::string& culture) const;
		std::optional<std::vector<std::string>> getFemaleNames(const std::string& culture) const;
		std::optional<std::vector<std::string>> getSurnames(const std::string& culture) const;
		std::optional<std::vector<std::string>> getFemaleSurnames(const std::string& culture) const;
		std::optional<std::vector<std::string>> getCallsigns(const std::string& culture) const;

		std::optional<std::string> getMaleName(const std::string& culture);
		std::optional<std::string> getFemaleName(const std::string& culture);
		std::optional<std::string> getSurname(const std::string& culture);
		std::optional<std::string> getFemaleSurname(const std::string& culture);
		std::optional<std::string> getCallsign(const std::string& culture);

		std::optional<std::string> takeCarCompanyName(const std::string& culture);
		std::optional<std::string> takeWeaponCompanyName(const std::string& culture);
		std::optional<std::string> takeAircraftCompanyName(const std::string& culture);
		std::optional<std::string> takeNavalCompanyName(const std::string& culture);
		std::optional<std::string> takeIndustryCompanyName(const std::string& culture);
		std::optional<std::string> takeElectronicCompanyName(const std::string& culture);

	private:
		namesMapper(const namesMapper&) = delete;
		namesMapper& operator=(const namesMapper&) = delete;

		void processVic2CulturesFile(const std::string& filename);
		void processNamesFile();
		void checkForNames();

		void addNamesToMap(std::map<std::string, std::vector<std::string>>& map, const std::string& culture, std::vector<std::string>& newNames);

		std::optional<std::string> takeCompanyName(std::map<std::string, std::vector<std::string>>& companyNames, const std::string& culture);

		std::map<std::string, std::vector<std::string>> maleNamesMap;
		std::map<std::string, std::vector<std::string>> femaleNamesMap;
		std::map<std::string, std::vector<std::string>> surnamesMap;
		std::map<std::string, std::vector<std::string>> femaleSurnamesMap;
		std::map<std::string, std::vector<std::string>> callsignsMap;
		std::map<std::string, std::vector<std::string>> carCompanyNames;
		std::map<std::string, std::vector<std::string>> weaponCompanyNames;
		std::map<std::string, std::vector<std::string>> aircraftCompanyNames;
		std::map<std::string, std::vector<std::string>> navalCompanyNames;
		std::map<std::string, std::vector<std::string>> industryCompanyNames;
		std::map<std::string, std::vector<std::string>> electronicCompanyNames;

		std::mt19937 rng;
};

}



#endif // NAMES_MAPPER_H