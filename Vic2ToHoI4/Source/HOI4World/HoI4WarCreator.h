/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include <map>
#include <string>
#include "HoI4Country.h"
using namespace std;


class HoI4World;



class HoI4WarCreator
{
	public:
		void generateWars(HoI4World* world);

	private:
		void determineProvinceOwners();
		void fillCountryProvinces();
		void addAllTargetsToWorldTargetMap();
		void addTargetsToWorldTargetMap(HoI4Country* country);
		map<double, HoI4Country*> getDistancesToGreatPowers(HoI4Country* country);
		double calculateWorldStrength(ofstream& AILog);
		void generateMajorWars(ofstream& AILog, set<HoI4Faction*>& factionsAtWar);
		double calculatePercentOfWorldAtWar(ofstream& AILog, const set<HoI4Faction*>& factionsAtWar, double worldStrength);
		void generateAdditionalWars(ofstream& AILog, set<HoI4Faction*>& factionsAtWar, double worldStrength);
		bool isImportantCountry(HoI4Country* country);

		vector<HoI4Faction*> fascistWarMaker(HoI4Country* country, ofstream& AILog);
		vector<HoI4Faction*> communistWarCreator(HoI4Country* country, ofstream& AILog);
		vector<HoI4Faction*> democracyWarCreator(HoI4Country* country);
		vector<HoI4Faction*> absolutistWarCreator(HoI4Country* country);
		vector<HoI4Faction*> neighborWarCreator(HoI4Country* country, ofstream& AILog);

		vector<HoI4Faction*> radicalWarCreator(HoI4Country* country);
		vector<HoI4Country*> findEvilCountries();

		vector<HoI4Country*> findWeakNeighbors(const HoI4Country* country);
		map<string, HoI4Country*> findCloseNeighbors(const HoI4Country* country);
		vector<HoI4Country*> findWeakColonies(const HoI4Country* country);
		map<string, HoI4Country*> findFarNeighbors(const HoI4Country* country);
		vector<HoI4Country*> getGreatPowerTargets(HoI4Country* country);
		map<double, HoI4Country*> getGPsByDistance(const HoI4Country* country);
		vector<HoI4Faction*> addGreatPowerWars(HoI4Country* country, HoI4FocusTree* FocusTree, const vector<HoI4Country*>& greatPowerTargets);
		void addTradeEvents(const HoI4Country* country, const vector<HoI4Country*>& greatPowerTargets);

		void    setSphereLeaders(const V2World* sourceWorld);
		HoI4Country* FindProvOwner(int prov);
		double    GetFactionStrength(HoI4Faction* Faction, int years);
		string HowToTakeLand(HoI4Country * TargetCountry, HoI4Country * AttackingCountry, double time);
		vector<HoI4Country*> GetMorePossibleAllies(HoI4Country * CountryThatWantsAllies);
		double getDistanceBetweenCountries(const HoI4Country* Country1, const HoI4Country* Country2);
		bool bothCountriesHaveCapitals(const HoI4Country* Country1, const HoI4Country* Country2);
		pair<int, int> getCapitalPosition(const HoI4Country* country);
		pair<int, int> getProvincePosition(int provinceNum);
		void establishProvincePositions();
		void processPositionLine(const string& line);
		vector<string> tokenizeLine(const string& line);
		void addProvincePosition(const vector<string>& tokenizedLine);
		double getDistanceBetweenPoints(pair<int, int> point1, pair<int, int> point2);
		double GetFactionStrengthWithDistance(HoI4Country* HomeCountry, vector<HoI4Country*> Faction, double time);
		HoI4Faction* findFaction(HoI4Country * checkingCountry);

		map<string, HoI4Country*> getNeighbors(const HoI4Country* checkingCountry);
		map<string, HoI4Country*> getImmediateNeighbors(const HoI4Country* checkingCountry);
		map<string, HoI4Country*> getNearbyCountries(const HoI4Country* checkingCountry);


		HoI4FocusTree* genericFocusTree;
		HoI4World* theWorld;

		vector<HoI4Country*> AggressorFactions;
		map<HoI4Country*, vector<HoI4Country*>> WorldTargetMap;
		map<int, pair<int, int>> provincePositions;

		string axisLeader;
		string alliesLeader;
		string cominternLeader;
		map<int, string> provinceToOwnerMap;
};