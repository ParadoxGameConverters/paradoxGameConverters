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



#ifndef HOI4STATE_H_
#define HOI4STATE_H_



#include <vector>
#include <string>
#include <map>
#include <set>
using namespace std;



class V2Province;
class Vic2State;



class HoI4State
{
	public:
		HoI4State(const Vic2State* sourceState, int _ID, string _ownerTag);

		void output(string filename);

		void addProvince(int province) { provinces.insert(province); }
		void setAsCapitalState() { capitalState = true; civFactories++; }
		void addResource(string resource, double amount)	{ resources[resource] += amount; }
		void addAirBase(int newAirBase) { airbaseLevel += newAirBase; if (airbaseLevel > 10) airbaseLevel = 10; }
		void addVictoryPointValue(int additionalValue) { victoryPointValue += additionalValue; }
		void setVPLocation(int province) { victoryPointPosition = province; }

		void convertNavalBases();
		void addNavalBase(int level, int location);
		void addCores(const vector<string>& newCores);

		const Vic2State* getSourceState() const { return sourceState; }
		set<int>	getProvinces() const { return provinces; }
		string getOwner() const { return ownerTag; }
		set<string> getCores() const { return cores; }
		int getID() const { return ID; }
		int getDockyards() const { return dockyards; }
		int getCivFactories() const { return civFactories; }
		int getMilFactories() const { return milFactories; }
		int getInfrastructure() const { return infrastructure; }
		int getManpower() const { return manpower; }
		int getVPLocation() const { return victoryPointPosition; }

		int getMainNavalLocation() const;

		void tryToCreateVP();
		void addManpower();

		void convertIndustry(double workerFactoryRatio);

	private:
		int determineFactoryNumbers(double workerFactoryRatio);
		int constrainFactoryNumbers(double rawFactories);
		void determineCategory(int factories);
		void setInfrastructure(int factories);
		void setIndustry(int factories);
		bool amICoastal();

		int determineNavalBaseLevel(const V2Province* sourceProvince);
		int determineNavalBaseLocation(const V2Province* sourceProvince);

		bool assignVPFromVic2Province(int Vic2ProvinceNumber);
		void assignVP(int location);
		bool isProvinceInState(int provinceNum);

		const Vic2State* sourceState;

		int ID;
		set<int> provinces;
		string ownerTag;
		set<string> cores;
		bool capitalState;

		int manpower;

		int civFactories;
		int milFactories;
		int dockyards;
		string category;
		int infrastructure;
	
		vector<pair<int, int>> navalBases;

		int airbaseLevel;

		map<string, double> resources;

		int victoryPointPosition;
		int victoryPointValue;
};



#endif // HOI4STATE_H_
