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



#ifndef V2WORLD_H_
#define V2WORLD_H_



#include "V2Inventions.h"
#include "../Mappers/Mapper.h"
#include "Object.h"
#include <string>
#include <vector>
using namespace std;


class V2Country;
class V2Diplomacy;
class V2Province;
struct V2Party;


class V2World
{
	public:
		V2World(const string& filename);

		const V2Province* getProvince(int provNum) const;
		
		const map<string, V2Country*>& getCountries() const { return countries; }
		const V2Diplomacy* getDiplomacy() const { return diplomacy; }
		const vector<string>& getGreatPowers() const	{ return greatPowers; }
		const vector<V2Party*>& getParties() const { return parties; }

	private:
		void setLocalisations();

		map<int, int> extractGreatNationIndices(const shared_ptr<Object> obj);

		bool isProvinceKey(string key) const;
		bool isCountryKey(string key) const;
		bool isNormalCountryKey(string key) const;
		bool isDominionCountryKey(string key) const;
		bool isConvertedCountryKey(string key) const;

		void setGreatPowerStatus(string tag, const map<int, int>& greatNationIndices, const unsigned int& countriesIndex);

		void setProvinceOwners();
		void addProvinceCoreInfoToCountries();
		void removeSimpleLandlessNations();
		bool shouldCoreBeRemoved(const V2Province* core, const V2Country* country);
		void determineEmployedWorkers();
		void removeEmptyNations();
		void determinePartialStates();
		void inputDiplomacy(const vector<shared_ptr<Object>> diplomacyObj);

		void overallMergeNations();
		void mergeNations(string masterTag, const vector<string>& slaveTags);

		void checkAllProvincesMapped() const;

		void readCountryFiles();
		bool processCountriesDotTxt(string countryListFile, string mod);
		bool shouldLineBeSkipped(string line) const;
		string extractCountryFileName(string countryFileLine) const;
		shared_ptr<Object> readCountryFile(string countryFileName, string mod) const;
		void readCountryColor(const shared_ptr<Object> countryData, string line);
		void inputPartyInformation(const vector<shared_ptr<Object>>& leaves);

		V2Country* getCountry(string tag) const;


		map<int, V2Province*> provinces;
		map<string, V2Country*>	countries;
		V2Diplomacy* diplomacy;
		vector<V2Party*> parties;
		vector<string> greatPowers;
};



#endif // V2WORLD_H_