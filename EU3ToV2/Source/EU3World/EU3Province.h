/*Copyright (c) 2014 The Paradox Game Converters Project

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


#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include "../Date.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class Object;
class EU3Country;



struct EU3PopRatio {
	string culture;
	string religion;
	double popRatio;
};


class EU3Province {
	public:
		EU3Province(Object* obj);

		void						addCore(string tag);
		void						removeCore(string tag);
		void						determineProvinceWeight();

		bool						wasColonised() const;
		bool						wasInfidelConquest() const;
		bool						hasBuilding(string building) const;

		vector<EU3Country*>	getCores(const map<string, EU3Country*>& countries) const;
		date						getLastPossessedDate(string tag) const;

		int						getNum()					const { return num; };
		double					getBaseTax()			const { return baseTax; }
		string					getOwnerString()		const { return ownerString; };
		EU3Country*				getOwner()				const { return owner; };
		int						getPopulation()		const { return population; };
		bool						isColony()				const { return colony; };
		bool						isCOT()					const { return centerOfTrade; };
		vector<EU3PopRatio>	getPopRatios()			const { return popRatios; };
		double					getTotalWeight()		const { return totalWeight; }
		int						getNumDestV2Provs()	const { return numV2Provs; };

		void						setTotalWeight(double totalWeight);
		void						checkManpower(const Object* provinceObj);
		void						checkTradeGoods(const Object* provinceObj);

		void						setOwner(EU3Country* newOwner)		{ owner = newOwner; }
		void						setNumDestV2Provs(int _numV2Provs)	{ numV2Provs = _numV2Provs; }
		string					getProvName() const						{ return provName; }

		// getters for weight attributes
		double						getProvTaxIncome()				const	{ return provTaxIncome; }
		double						getProvProdIncome()				const	{ return provProdIncome; }
		double						getProvMPWeight()					const	{ return provMPWeight; }
		double						getProvTotalBuildingWeight()	const	{ return provBuildingWeight; }
		double						getCurrTradeGoodWeight()		const	{ return provTradeGoodWeight; }
		std::vector<double>		getProvProductionVec()			const	{ return provProductionVec; }
		string						getTradeGoods()					const { return tradeGoods; }

		void						setCOT(bool isCOT)					{ centerOfTrade = isCOT; };
	private:
		void	checkBuilding(const Object* provinceObj, string building);
		void	buildPopRatios();
		void	decayPopRatios(date olddate, date newdate, EU3PopRatio& currentPop);

		vector<double>	getProvBuildingWeight()	const;
		double			getTradeGoodWeight()		const;
		double			getTradeGoodPrice()		const;

		int									num;
		double								baseTax;
		double								totalWeight;
		string								ownerString;
		string								provName;
		EU3Country*							owner;
		vector<string>						cores;
		int									population;
		bool									colony;
		bool									centerOfTrade;
		vector< pair<date, string> >	ownershipHistory;
		map<string, date>					lastPossessedDate;
		vector< pair<date, string> >	religionHistory;
		vector< pair<date, string> >	cultureHistory;
		vector<EU3PopRatio>				popRatios;
		map<string, bool>					buildings;
		double								manpower;
		string								tradeGoods;
		int									numV2Provs;

		// province attributes for weights
		double								provTaxIncome;
		double								provProdIncome;
		double								provMPWeight;
		double								provBuildingWeight;
		double								provTradeGoodWeight;

		std::vector<double>					provProductionVec;
};


#endif // EU3PROVINCE_H_