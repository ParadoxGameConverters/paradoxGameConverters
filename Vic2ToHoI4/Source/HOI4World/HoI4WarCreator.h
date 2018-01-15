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



#include <fstream>
#include <optional>
#include <map>
#include <string>
#include "HoI4Country.h"
using namespace std;



class HoI4World;



const int maxGCWars = 1;



class HoI4WarCreator
{
	public:
		explicit HoI4WarCreator(const HoI4World* world);

	private:
		HoI4WarCreator(const HoI4WarCreator&) = delete;
		HoI4WarCreator& operator=(const HoI4WarCreator&) = delete;

		void determineProvinceOwners();
		void fillCountryProvinces();
		void addAllTargetsToWorldTargetMap();
		void addTargetsToWorldTargetMap(shared_ptr<HoI4Country> country);
		map<double, shared_ptr<HoI4Country>> getDistancesToGreatPowers(shared_ptr<HoI4Country> country);
		double calculateWorldStrength(ofstream& AILog) const;
		void generateMajorWars(ofstream& AILog, set<shared_ptr<HoI4Faction>>& factionsAtWar, const HoI4World* world);
		double calculatePercentOfWorldAtWar(ofstream& AILog, const set<shared_ptr<HoI4Faction>>& factionsAtWar, double worldStrength) const;
		void generateAdditionalWars(ofstream& AILog, set<shared_ptr<HoI4Faction>>& factionsAtWar, double worldStrength);
		bool isImportantCountry(shared_ptr<HoI4Country> country);

		vector<shared_ptr<HoI4Faction>> fascistWarMaker(shared_ptr<HoI4Country> country, ofstream& AILog, const HoI4World *world);
		vector<shared_ptr<HoI4Faction>> communistWarCreator(shared_ptr<HoI4Country> country, ofstream& AILog);
		vector<shared_ptr<HoI4Faction>> democracyWarCreator(shared_ptr<HoI4Country> country);
		vector<shared_ptr<HoI4Faction>> absolutistWarCreator(shared_ptr<HoI4Country> country);
		vector<shared_ptr<HoI4Faction>> neighborWarCreator(shared_ptr<HoI4Country> country, ofstream& AILog);
		vector<shared_ptr<HoI4Faction>> radicalWarCreator(shared_ptr<HoI4Country> country);

		vector<shared_ptr<HoI4Country>> findEvilCountries() const;

		set<int> findBorderState(shared_ptr<HoI4Country> country, shared_ptr<HoI4Country> neighbor, const HoI4World* world);
		vector<int> sortStatesByCapitalDistance(set<int> stateList, shared_ptr<HoI4Country> country, const HoI4World* world);
		vector<shared_ptr<HoI4Country>> findWeakNeighbors(shared_ptr<HoI4Country> country);
		map<string, shared_ptr<HoI4Country>> findCloseNeighbors(shared_ptr<HoI4Country> country);
		vector<shared_ptr<HoI4Country>> findWeakColonies(shared_ptr<HoI4Country> country);
		map<string, shared_ptr<HoI4Country>> findFarNeighbors(shared_ptr<HoI4Country> country);
		vector<shared_ptr<HoI4Country>> getGreatPowerTargets(shared_ptr<HoI4Country> country);
		map<double, shared_ptr<HoI4Country>> getGPsByDistance(shared_ptr<HoI4Country> country);
		vector<shared_ptr<HoI4Faction>> addGreatPowerWars(shared_ptr<HoI4Country> country, shared_ptr<HoI4FocusTree> FocusTree, vector<shared_ptr<HoI4Country>>& greatPowerTargets);
		void addTradeEvents(shared_ptr<HoI4Country> country, const vector<shared_ptr<HoI4Country>>& greatPowerTargets);

		void    setSphereLeaders(const V2World* sourceWorld);
		double    GetFactionStrength(const shared_ptr<HoI4Faction> Faction, int years) const;
		string HowToTakeLand(shared_ptr<HoI4Country> TargetCountry, shared_ptr<HoI4Country> AttackingCountry, double time);
		vector<shared_ptr<HoI4Country>> GetMorePossibleAllies(shared_ptr<HoI4Country> CountryThatWantsAllies);
		optional<double> getDistanceBetweenCountries(shared_ptr<HoI4Country> Country1, shared_ptr<HoI4Country> Country2);
		bool bothCountriesHaveCapitals(shared_ptr<HoI4Country> Country1, shared_ptr<HoI4Country> Country2) const;
		pair<int, int> getCapitalPosition(shared_ptr<HoI4Country> country);
		pair<int, int> getProvincePosition(int provinceNum);
		void establishProvincePositions();
		void processPositionLine(const string& line);
		vector<string> tokenizeLine(const string& line);
		void addProvincePosition(const vector<string>& tokenizedLine);
		double getDistanceBetweenPoints(pair<int, int> point1, pair<int, int> point2) const;
		double GetFactionStrengthWithDistance(shared_ptr<HoI4Country> HomeCountry, vector<shared_ptr<HoI4Country>> Faction, double time);
		shared_ptr<HoI4Faction> findFaction(shared_ptr<HoI4Country> checkingCountry);

		map<string, shared_ptr<HoI4Country>> getNeighbors(shared_ptr<HoI4Country> checkingCountry);
		map<string, shared_ptr<HoI4Country>> getImmediateNeighbors(shared_ptr<HoI4Country> checkingCountry);
		map<string, shared_ptr<HoI4Country>> getNearbyCountries(shared_ptr<HoI4Country> checkingCountry);


		HoI4FocusTree* genericFocusTree;
		const HoI4World* theWorld;

		vector<HoI4Country*> AggressorFactions;
		map<shared_ptr<HoI4Country>, vector<shared_ptr<HoI4Country>>> WorldTargetMap;
		map<int, pair<int, int>> provincePositions;

		map<int, string> provinceToOwnerMap;
};