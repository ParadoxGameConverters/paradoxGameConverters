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



#include <map>
#include <string>
#include <vector>
#include "HoI4Localisation.h"
#include "HoI4State.h"
#include "..\V2World\V2Province.h"
#include "../Mappers/ProvinceMapper.h"
using namespace std;



struct MTo1ProvinceComp
{
	MTo1ProvinceComp() : totalPopulation(0) {};

	vector<const V2Province*> provinces;
	int totalPopulation;
};


typedef struct ownersAndCores
{
	string owner;
	vector<string> cores;
} ownersAndCores;


class HoI4States
{
	public:
		HoI4States(const V2World* srcWorld): sourceWorld(srcWorld) {};

		void importStates(map<int, vector<int>>& defaultStateToProvinceMap);
		void convertStates();
		void addLocalisations();

		const map<int, HoI4State*>& getStates() const { return states; }
		const map<int, int>& getProvinceToStateIDMap() const { return provinceToStateIDMap; }

		void output() const;

	private:
		map<int, ownersAndCores> determineProvinceOwners();
		bool getAppropriateMapping(int provNum, HoI4ToVic2ProvinceMapping::const_iterator& provinceLink);
		map<const V2Country*, MTo1ProvinceComp> determinePotentialOwners(HoI4ToVic2ProvinceMapping::const_iterator provinceLink);
		const V2Country* selectProvinceOwner(const map<const V2Country*, MTo1ProvinceComp>& provinceBins);
		void createStates(const map<int, ownersAndCores>& provinceToOwnersAndCoresMap);
		bool createMatchingHoI4State(const Vic2State* vic2State, int stateID, const string& stateOwner, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces);
		void addProvincesToNewState(HoI4State* newState, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces);
		bool isProvinceOwnedByCountryAndNotAlreadyAssigned(int provNum, string stateOwner, const map<int, ownersAndCores>& provinceToOwnersAndCoresMap, set<int>& assignedProvinces);
		void createVPForState(HoI4State* newState);
		void addManpowerToNewState(HoI4State* newState);
		void addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		void addVPLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		keyToLocalisationMap& getExistingStateLocalisation(const string& language);
		keyToLocalisationMap& getExistingVPLocalisation(const string& language);
		void addLanguageToStateLocalisations(const string& language);
		void addLanguageToVPLocalisations(const string& language);
		void addNonenglishStateLocalisations();
		void addNonenglishVPLocalisations();
		unsigned int getTotalManpower();

		void outputHistory() const;
		void outputStateLocalisations() const;
		void outputVPLocalisations() const;

		const V2World* sourceWorld;

		map<int, HoI4State*> states;
		map<int, int> provinceToStateIDMap;

		languageToLocalisationsMap stateLocalisations;
		languageToLocalisationsMap VPLocalisations;
};



#endif // HOI4_STATES_H_
