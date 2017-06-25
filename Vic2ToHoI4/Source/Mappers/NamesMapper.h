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
		static vector<string> getMaleNames(string culture)
		{
			return getInstance()->GetMaleNames(culture);
		}
		static vector<string> getFemaleNames(string culture)
		{
			return getInstance()->GetFemaleNames(culture);
		}
		static vector<string> getSurnames(string culture)
		{
			return getInstance()->GetSurnames(culture);
		}
		static vector<string> getCallsigns(string culture)
		{
			return getInstance()->GetCallsigns(culture);
		}

		static string getMaleName(string culture)
		{
			return getInstance()->GetMaleName(culture);
		}
		static string getFemaleName(string culture)
		{
			return getInstance()->GetFemaleName(culture);
		}
		static string getSurname(string culture)
		{
			return getInstance()->GetSurname(culture);
		}
		static string getCallsign(string culture)
		{
			return getInstance()->GetCallsign(culture);
		}
		static string getCarCompanyName(string culture)
		{
			return getInstance()->GetCarCompanyName(culture);
		}
		static string getWeaponCompanyName(string culture)
		{
			return getInstance()->GetWeaponCompanyName(culture);
		}
		static string getAircraftCompanyName(string culture)
		{
			return getInstance()->GetAircraftCompanyName(culture);
		}
		static string getNavalCompanyName(string culture)
		{
			return getInstance()->GetNavalCompanyName(culture);
		}
		static string getIndustryCompanyName(string culture)
		{
			return getInstance()->GetIndustryCompanyName(culture);
		}
		static string getElectronicCompanyName(string culture)
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

		void processVic2CulturesFile(string filename);
		void processNamesFile();
		void checkForNames();

		vector<string> GetMaleNames(string culture) const;
		vector<string> GetFemaleNames(string culture) const;
		vector<string> GetSurnames(string culture) const;
		vector<string> GetCallsigns(string culture) const;
		string GetMaleName(string culture);
		string GetFemaleName(string culture);
		string GetSurname(string culture);
		string GetCallsign(string culture);
		string GetCarCompanyName(string culture);
		string GetWeaponCompanyName(string culture);
		string GetAircraftCompanyName(string culture);
		string GetNavalCompanyName(string culture);
		string GetIndustryCompanyName(string culture);
		string GetElectronicCompanyName(string culture);
		string getCompanyName(map<string, vector<string>>& companyNames, string culture);

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