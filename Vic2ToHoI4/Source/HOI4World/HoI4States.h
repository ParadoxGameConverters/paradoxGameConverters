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


#ifndef HOI4_STATES_H_
#define HOI4_STATES_H_



#include "../Mappers/ProvinceMapper.h"
#include "newParser.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;



class CountryMapper;
namespace HoI4
{
class impassableProvinces;
class State;
}
namespace Vic2
{
class Country;
class State;
class World;
}



class HoI4States: commonItems::parser
{
	public:
		explicit HoI4States(const Vic2::World* _sourceWorld, const CountryMapper& countryMap);

		const map<int, HoI4::State*>& getDefaultStates() const { return defaultStates; }
		const map<int, HoI4::State*>& getStates() const { return states; }
		const map<int, int>& getProvinceToStateIDMap() const { return provinceToStateIDMap; }

		void output() const;

	private:
		HoI4States(const HoI4States&) = delete;
		HoI4States& operator=(const HoI4States&) = delete;

		void determineOwnersAndCores(const CountryMapper& countryMap);
		optional<vector<int>> retrieveSourceProvinceNums(int provNum) const;
		map<const Vic2::Country*, pair<int, int>> determinePotentialOwners(const vector<int>& sourceProvinceNums) const;
		const Vic2::Country* selectProvinceOwner(const map<const Vic2::Country*, pair<int, int>>& potentialOwners) const;
		vector<string> determineCores(const vector<int>& sourceProvinces, const Vic2::Country* oldOwner, const CountryMapper& countryMap, const std::string& newOwner) const;

		void createStates(const HoI4::impassableProvinces& theImpassables, const CountryMapper& countryMap);
		void createMatchingHoI4State(const Vic2::State* vic2State, const string& stateOwner, const HoI4::impassableProvinces& theImpassables);
		std::unordered_set<int> getProvincesInState(const Vic2::State* vic2State, const string& owner);
		void addProvincesAndCoresToNewState(HoI4::State* newState, unordered_set<int> provinces);
		bool isProvinceValid(int provNum) const;
		bool isProvinceOwnedByCountry(int provNum, const string& stateOwner) const;
		bool isProvinceNotAlreadyAssigned(int provNum) const;
		unsigned int getTotalManpower() const;


		const Vic2::World* sourceWorld = nullptr;
		map<int, string> ownersMap;
		map<int, vector<string>> coresMap;
		set<int> assignedProvinces;

		map<int, HoI4::State*> defaultStates;
		map<int, HoI4::State*> states;
		map<int, int> provinceToStateIDMap;
		int nextStateID = 1;
};



#endif // HOI4_STATES_H_
