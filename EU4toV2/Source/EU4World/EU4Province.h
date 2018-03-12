/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#ifndef EU4PROVINCE_H_
#define EU4PROVINCE_H_



#include "Date.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
using namespace std;



class Object;

namespace EU4
{
	class Country;
}



struct EU4PopRatio
{
	string culture;			// the culture
	string religion;			// the religion
	double upperPopRatio;	// the percent of the total upper-class population this represents
	double middlePopRatio;	// the percent of the total middle-class population this represents
	double lowerPopRatio;	// the percent of the total lower-class population this represents
};


class EU4Province
{
	public:
		EU4Province(shared_ptr<Object> obj);

		void						addCore(string tag);
		void						removeCore(string tag);
		void						determineProvinceWeight();

		bool						wasColonised() const;
		bool						wasInfidelConquest() const;
		bool						hasBuilding(string building) const;
		std::vector<std::shared_ptr<EU4::Country>>	getCores(const std::map<std::string, std::shared_ptr<EU4::Country>>& countries) const;
		date						getLastPossessedDate(string tag) const;
		double getCulturePercent(string culture);

		int						getNum()					const { return num; }
		double					getBaseTax()			const { return baseTax; }
		string					getOwnerString()		const { return ownerString; }
		std::shared_ptr<EU4::Country> getOwner() const { return owner; }
		bool						getInHRE()				const { return inHRE; }
		bool						isColony()				const { return colony; }
		vector<EU4PopRatio>	getPopRatios()			const { return popRatios; }
		double					getTotalWeight()		const { return totalWeight; }
		int						getNumDestV2Provs()	const { return numV2Provs; }

		void						setOwner(std::shared_ptr<EU4::Country> newOwner) { owner = newOwner; }
		void						setNumDestV2Provs(int _numV2Provs)	{ numV2Provs = _numV2Provs; }
		string					getProvName() const { return provName; }

		// getters for weight attributes
		double					getProvTaxIncome() const { return provTaxIncome; }
		double					getProvProdIncome() const { return provProdIncome; }
		double					getProvMPWeight() const { return provMPWeight; }
		double					getProvTotalBuildingWeight() const { return provBuildingWeight; }
		double					getProvTotalDevModifier() const { return provDevModifier; }
		double					getCurrTradeGoodWeight() const { return provTradeGoodWeight; }

		std::vector<double>	getProvProductionVec() const { return provProductionVec; }
		string					getTradeGoods() const { return tradeGoods; }

	private:
		void	checkBuilding(const shared_ptr<Object> provinceObj, string building);
		void	buildPopRatios();
		void	decayPopRatios(date oldDate, date newDate, EU4PopRatio& currentPop);

		vector<double>	getProvBuildingWeight()	const;
		double			getTradeGoodWeight()		const;
		double			getTradeGoodPrice()		const;

		int									num;						// the province number
		double								baseTax;					// the base tax of the province
		double								baseProd;					// the base production of the province
		double								manpower;					// the base manpower of the province
		double								totalWeight;
		string								ownerString;			// a string with the owner's tag
		string								provName;
		std::shared_ptr<EU4::Country> owner;
		vector<string>						cores;					// strings of the tags of all cores
		bool									inHRE;					// whether or not this province is in the HRE
		bool									colony;					// whether or not this is a colony
		vector< pair<date, string> >	ownershipHistory;		// the history of who has owned this province
		map<string, date>					lastPossessedDate;	// the last date the province was owned by different tags
		vector< pair<date, string> >	religionHistory;		// the history of the religious changes of this province
		vector< pair<date, string> >	cultureHistory;		// the history of the cultural changes of this province
		vector<EU4PopRatio>				popRatios;				// the population ratios of this province
		map<string, bool>					buildings;				// the buildings in this province

		string								tradeGoods;
		int									numV2Provs;

		// province attributes for weights
		double								provTaxIncome;
		double								provProdIncome;
		double								provMPWeight;
		double								provBuildingWeight;
		double								provTradeGoodWeight;
		double								provDevModifier;

		std::vector<double>				provProductionVec;

};



#endif // EU4PROVINCE_H_
