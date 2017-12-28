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



#include "../Mappers/Mapper.h"
#include "Object.h"
#include <optional>
#include <string>
#include <vector>
using namespace std;


class V2Country;
class V2Diplomacy;
class V2Province;
class V2Party;


class V2World
{
	public:
		explicit V2World(const string& filename);

		optional<const V2Province*> getProvince(int provNum) const;
		
		map<string, V2Country*> getCountries() const { return countries; }
		const V2Diplomacy* getDiplomacy() const { return diplomacy; }
		vector<string> getGreatPowers() const	{ return greatPowers; }
		vector<const V2Party*> getParties() const { return parties; }

	private:
		V2World(const V2World&) = delete;
		V2World& operator=(const V2World&) = delete;

		void setLocalisations();
		void handleMissingCountryCultures();

		map<int, int> extractGreatNationIndices(const shared_ptr<Object> obj) const;

		bool isProvinceKey(const string& key) const;
		bool isCountryKey(const string& key) const;
		bool isNormalCountryKey(const string& key) const;
		bool isDominionCountryKey(const string& key) const;
		bool isConvertedCountryKey(const string& key) const;

		void setGreatPowerStatus(const string& tag, const map<int, int>& greatNationIndices, unsigned int countriesIndex);

		void setProvinceOwners();
		void addProvinceCoreInfoToCountries();
		void removeSimpleLandlessNations();
		bool shouldCoreBeRemoved(const V2Province* core, const V2Country* country) const;
		void determineEmployedWorkers();
		void removeEmptyNations();
		void determinePartialStates();
		void inputDiplomacy(const shared_ptr<Object>& diplomacyObj);

		void overallMergeNations();
		void mergeNations(const string& masterTag, const vector<string>& slaveTags);

		void checkAllProvincesMapped() const;

		void readCountryFiles();
		bool processCountriesDotTxt(const string& countryListFile, const string& mod);
		bool shouldLineBeSkipped(const string& line) const;
		string extractCountryFileName(const string& countryFileLine) const;
		shared_ptr<Object> readCountryFile(const string& countryFileName, const string& mod) const;
		void readCountryColor(shared_ptr<Object> countryData, const string& line);
		void inputPartyInformation(const vector<shared_ptr<Object>>& leaves);

		optional<V2Country*> getCountry(const string& tag) const;


		map<int, V2Province*> provinces;
		map<string, V2Country*>	countries;
		const V2Diplomacy* diplomacy;
		vector<const V2Party*> parties;
		vector<string> greatPowers;
};



#endif // V2WORLD_H_