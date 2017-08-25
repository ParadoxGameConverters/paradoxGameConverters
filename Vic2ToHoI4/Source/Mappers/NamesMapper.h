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



#include <map>
#include <random>
#include <string>
#include <vector>
using namespace std;



class namesMapper
{
	public:
		static vector<string> getMaleNames(const string& culture)
		{
			return getInstance()->GetMaleNames(culture);
		}
		static vector<string> getFemaleNames(const string& culture)
		{
			return getInstance()->GetFemaleNames(culture);
		}
		static vector<string> getSurnames(const string& culture)
		{
			return getInstance()->GetSurnames(culture);
		}
		static vector<string> getCallsigns(const string& culture)
		{
			return getInstance()->GetCallsigns(culture);
		}

		static string getMaleName(const string& culture)
		{
			return getInstance()->GetMaleName(culture);
		}
		static string getFemaleName(const string& culture)
		{
			return getInstance()->GetFemaleName(culture);
		}
		static string getSurname(const string& culture)
		{
			return getInstance()->GetSurname(culture);
		}
		static string getCallsign(const string& culture)
		{
			return getInstance()->GetCallsign(culture);
		}
		static string getCarCompanyName(const string& culture)
		{
			return getInstance()->GetCarCompanyName(culture);
		}
		static string getWeaponCompanyName(const string& culture)
		{
			return getInstance()->GetWeaponCompanyName(culture);
		}
		static string getAircraftCompanyName(const string& culture)
		{
			return getInstance()->GetAircraftCompanyName(culture);
		}
		static string getNavalCompanyName(const string& culture)
		{
			return getInstance()->GetNavalCompanyName(culture);
		}
		static string getIndustryCompanyName(const string& culture)
		{
			return getInstance()->GetIndustryCompanyName(culture);
		}
		static string getElectronicCompanyName(const string& culture)
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
		const namesMapper& operator=(const namesMapper&) = delete;

		void processVic2CulturesFile(const string& filename);
		void processNamesFile();
		void checkForNames();

		vector<string> GetMaleNames(const string& culture) const;
		vector<string> GetFemaleNames(const string& culture) const;
		vector<string> GetSurnames(const string& culture) const;
		vector<string> GetCallsigns(const string& culture) const;
		string GetMaleName(const string& culture);
		string GetFemaleName(const string& culture);
		string GetSurname(const string& culture);
		string GetCallsign(const string& culture);
		string GetCarCompanyName(const string& culture);
		string GetWeaponCompanyName(const string& culture);
		string GetAircraftCompanyName(const string& culture);
		string GetNavalCompanyName(const string& culture);
		string GetIndustryCompanyName(const string& culture);
		string GetElectronicCompanyName(const string& culture);
		string getCompanyName(map<string, vector<string>>& companyNames, const string& culture);

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