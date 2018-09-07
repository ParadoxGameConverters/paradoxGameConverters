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



#ifndef HOI4STATE_H_
#define HOI4STATE_H_



#include "newParser.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>


namespace Vic2
{
class Province;
class State;
}


namespace HoI4
{

class coastalProvinces;
class stateCategories;


class State: commonItems::parser
{
	public:
		explicit State(std::istream& theStream);
		State(const Vic2::State* sourceState, int _ID, const std::string& _ownerTag);

		void output(const std::string& filename) const;

		void addProvince(int province) { provinces.insert(province); }
		void setAsCapitalState() { capitalState = true; civFactories++; }
		void makeImpassable() { impassable = true; }
		void markHadImpassablePart() { hadImpassablePart = true; }
		void addResource(const std::string& resource, double amount)	{ resources[resource] += amount; }
		void addAirBase(int newAirBase) { airbaseLevel += newAirBase; if (airbaseLevel > 10) airbaseLevel = 10; }
		void addVictoryPointValue(int additionalValue) { victoryPointValue += additionalValue; }
		void setVPLocation(int province) { victoryPointPosition = province; }

		void convertNavalBases(const coastalProvinces& theCoastalProvinces);
		void addNavalBase(int level, int location);
		void addCores(const std::vector<std::string>& newCores);

		const Vic2::State* getSourceState() const { return sourceState; }
		int getID() const { return ID; }
		std::set<int>	getProvinces() const { return provinces; }
		std::string getOwner() const { return ownerTag; }
		std::set<std::string> getCores() const { return cores; }
		bool isImpassable() const { return impassable; }
		int getDockyards() const { return dockyards; }
		int getCivFactories() const { return civFactories; }
		int getMilFactories() const { return milFactories; }
		int getInfrastructure() const { return infrastructure; }
		int getManpower() const { return manpower; }
		int getVPLocation() const { return victoryPointPosition; }
		std::set<int> getDebugVPs() const { return debugVictoryPoints; }
		std::set<int> getSecondaryDebugVPs() const { return secondaryDebugVictoryPoints; }

		std::optional<int> getMainNavalLocation() const;

		void tryToCreateVP();
		void addManpower();

		void convertIndustry(double workerFactoryRatio, const HoI4::stateCategories& theStateCategories, const coastalProvinces& theCoastalProvinces);

	private:
		State(const State&) = delete;
		State& operator=(const State&) = delete;

		int determineFactoryNumbers(double workerFactoryRatio);
		int constrainFactoryNumbers(double rawFactories);
		void determineCategory(int factories, const HoI4::stateCategories& theStateCategories);
		void setInfrastructure(int factories);
		void setIndustry(int factories, const coastalProvinces& theCoastalProvinces);
		bool amICoastal(const coastalProvinces& theCoastalProvinces);

		int determineNavalBaseLevel(const Vic2::Province* sourceProvince);
		std::optional<int> determineNavalBaseLocation(const Vic2::Province* sourceProvince, const coastalProvinces& theCoastalProvinces);

		bool assignVPFromVic2Province(int Vic2ProvinceNumber);
		void assignVP(int location);
		bool isProvinceInState(int provinceNum);
		void addDebugVPs();

		const Vic2::State* sourceState;

		int ID = 0;
		std::set<int> provinces;
		std::string ownerTag;
		std::set<std::string> cores;
		bool capitalState = false;
		bool impassable = false;
		bool hadImpassablePart = false;

		int manpower = 0;

		int civFactories = 0;
		int milFactories = 0;
		int dockyards = 0;
		std::string category = "pastoral";
		int infrastructure = 0;

                std::map<int, int> navalBases;

                int airbaseLevel = 0;

		std::map<std::string, double> resources;

		int victoryPointPosition = 0;
		int victoryPointValue = 0;
		std::set<int> debugVictoryPoints;
		std::set<int> secondaryDebugVictoryPoints;

                static std::map<int, int> landFortLevels;
                static std::map<int, int> coastFortLevels;
};

}



#endif // HOI4STATE_H_
