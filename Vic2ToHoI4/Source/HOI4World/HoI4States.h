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


#ifndef HOI4_STATES_H_
#define HOI4_STATES_H_



#include "../Mappers/ProvinceMapper.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
using namespace std;



class HoI4State;
class V2Country;
class V2Province;
class Vic2State;
class V2World;



class HoI4States
{
	public:
		explicit HoI4States(const V2World* _sourceWorld);

		const map<int, HoI4State*>& getStates() const { return states; }
		const map<int, int>& getProvinceToStateIDMap() const { return provinceToStateIDMap; }

		void output() const;

	private:
		HoI4States(const HoI4States&) = delete;
		HoI4States& operator=(const HoI4States&) = delete;

		void determineOwnersAndCores();
		optional<vector<int>> retrieveSourceProvinceNums(int provNum) const;
		map<const V2Country*, pair<int, int>> determinePotentialOwners(const vector<int>& sourceProvinceNums) const;
		const V2Country* selectProvinceOwner(const map<const V2Country*, pair<int, int>>& potentialOwners) const;
		vector<string> determineCores(const vector<int>& sourceProvinces, const V2Country* oldOwner) const;

		void createStates();
		void createMatchingHoI4State(const Vic2State* vic2State, const string& stateOwner);
		unordered_set<int> getProvincesInState(const Vic2State* vic2State, const string& owner);
		void addProvincesAndCoresToNewState(HoI4State* newState, unordered_set<int> provinces);
		bool isProvinceValid(int provNum) const;
		bool isProvinceOwnedByCountry(int provNum, const string& stateOwner) const;
		bool isProvinceNotAlreadyAssigned(int provNum) const;
		unsigned int getTotalManpower() const;


		const V2World* sourceWorld;
		map<int, string> ownersMap;
		map<int, vector<string>> coresMap;
		set<int> assignedProvinces;

		map<int, HoI4State*> states;
		map<int, int> provinceToStateIDMap;
		int nextStateID;
};



#endif // HOI4_STATES_H_
