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



#ifndef NAMES_MAPPER_H
#define NAMES_MAPPER_H



#include <optional>
#include <map>
#include <random>
#include <string>
#include <vector>
using namespace std;



class namesMapper
{
	public:
		static optional<vector<string>> getMaleNames(const string& culture)
		{
			return getInstance()->GetMaleNames(culture);
		}
		static optional<vector<string>> getFemaleNames(const string& culture)
		{
			return getInstance()->GetFemaleNames(culture);
		}
		static optional<vector<string>> getSurnames(const string& culture)
		{
			return getInstance()->GetSurnames(culture);
		}
		static optional<vector<string>> getCallsigns(const string& culture)
		{
			return getInstance()->GetCallsigns(culture);
		}

		static optional<string> getMaleName(const string& culture)
		{
			return getInstance()->GetMaleName(culture);
		}
		static optional<string> getFemaleName(const string& culture)
		{
			return getInstance()->GetFemaleName(culture);
		}
		static optional<string> getSurname(const string& culture)
		{
			return getInstance()->GetSurname(culture);
		}
		static optional<string> getCallsign(const string& culture)
		{
			return getInstance()->GetCallsign(culture);
		}
		static optional<string> getCarCompanyName(const string& culture)
		{
			return getInstance()->GetCarCompanyName(culture);
		}
		static optional<string> getWeaponCompanyName(const string& culture)
		{
			return getInstance()->GetWeaponCompanyName(culture);
		}
		static optional<string> getAircraftCompanyName(const string& culture)
		{
			return getInstance()->GetAircraftCompanyName(culture);
		}
		static optional<string> getNavalCompanyName(const string& culture)
		{
			return getInstance()->GetNavalCompanyName(culture);
		}
		static optional<string> getIndustryCompanyName(const string& culture)
		{
			return getInstance()->GetIndustryCompanyName(culture);
		}
		static optional<string> getElectronicCompanyName(const string& culture)
		{
			return getInstance()->GetElectronicCompanyName(culture);
		}
	private:
		static namesMapper* instance;
		static namesMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new namesMapper();
			}
			return instance;
		}
		namesMapper();

		namesMapper(const namesMapper&) = delete;
		namesMapper& operator=(const namesMapper&) = delete;

		void processVic2CulturesFile(const string& filename);
		void processNamesFile();
		void checkForNames();

		optional<vector<string>> GetMaleNames(const string& culture) const;
		optional<vector<string>> GetFemaleNames(const string& culture) const;
		optional<vector<string>> GetSurnames(const string& culture) const;
		optional<vector<string>> GetCallsigns(const string& culture) const;
		optional<string> GetMaleName(const string& culture);
		optional<string> GetFemaleName(const string& culture);
		optional<string> GetSurname(const string& culture);
		optional<string> GetCallsign(const string& culture);
		optional<string> GetCarCompanyName(const string& culture);
		optional<string> GetWeaponCompanyName(const string& culture);
		optional<string> GetAircraftCompanyName(const string& culture);
		optional<string> GetNavalCompanyName(const string& culture);
		optional<string> GetIndustryCompanyName(const string& culture);
		optional<string> GetElectronicCompanyName(const string& culture);
		optional<string> getCompanyName(map<string, vector<string>>& companyNames, const string& culture);

		map<string, vector<string>> maleNamesMap;
		map<string, vector<string>> femaleNamesMap;
		map<string, vector<string>> surnamesMap;
		map<string, vector<string>> callsignsMap;
		map<string, vector<string>> carCompanyNames;
		map<string, vector<string>> weaponCompanyNames;
		map<string, vector<string>> aircraftCompanyNames;
		map<string, vector<string>> navalCompanyNames;
		map<string, vector<string>> industryCompanyNames;
		map<string, vector<string>> electronicCompanyNames;

		std::mt19937 rng;
};





#endif // NAMES_MAPPER_H