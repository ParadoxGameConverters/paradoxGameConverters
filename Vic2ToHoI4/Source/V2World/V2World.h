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



#ifndef VIC2_WORLD_H_
#define VIC2_WORLD_H_



#include "Object.h"
#include "newParser.h"
#include "V2Party.h"
#include <optional>
#include <string>
#include <vector>



class V2Country;
class V2Diplomacy;
namespace Vic2
{

class Province;


class World: commonItems::parser
{
	public:
		explicit World(const string& filename);

		std::optional<const Vic2::Province*> getProvince(int provNum) const;
		
		std::map<std::string, V2Country*> getCountries() const { return countries; }
		const V2Diplomacy* getDiplomacy() const { return diplomacy; }
		std::vector<std::string> getGreatPowers() const	{ return greatPowers; }
		std::vector<V2Party> getParties() const { return parties; }
		auto getProvinces() const { return provinces; }

	private:
		World(const World&) = delete;
		World& operator=(const World&) = delete;

		void setLocalisations();
		void handleMissingCountryCultures();

		void setGreatPowerStatus(const std::vector<int>& GPIndexes, const std::vector<std::string>& tagsInOrder);

		void setProvinceOwners();
		void addProvinceCoreInfoToCountries();
		void removeSimpleLandlessNations();
		bool shouldCoreBeRemoved(const Vic2::Province* core, const V2Country* country) const;
		void determineEmployedWorkers();
		void removeEmptyNations();
		void determinePartialStates();

		void overallMergeNations();
		void mergeNations(const std::string& masterTag, const std::vector<std::string>& slaveTags);

		void checkAllProvincesMapped() const;

		void readCountryFiles();
		bool processCountriesDotTxt(const std::string& countryListFile, const std::string& mod);
		bool shouldLineBeSkipped(const std::string& line) const;
		std::string extractCountryFileName(const std::string& countryFileLine) const;
		shared_ptr<Object> readCountryFile(const std::string& countryFileName, const std::string& mod) const;
		void readCountryColor(std::shared_ptr<Object> countryData, const std::string& line);
		void readShipNames(std::shared_ptr<Object> countryData, const std::string& line);
		void inputPartyInformation(const std::vector<std::shared_ptr<Object>>& leaves);

		std::optional<V2Country*> getCountry(const std::string& tag) const;


		std::map<int, Vic2::Province*> provinces;
		std::map<std::string, V2Country*> countries;
		const V2Diplomacy* diplomacy = nullptr;
		std::vector<V2Party> parties;
		std::vector<std::string> greatPowers;
};


}



#endif // VIC2_WORLD_H_