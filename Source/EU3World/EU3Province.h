#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include <string>
#include <vector>
#include <map>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;



class Object;
class CK2Barony;
class CK2Province;
class EU3History;
class EU3Ruler;
class EU3Advisor;
class EU3Country;

class EU3Province
{
	public:
		EU3Province(int _num, Object* obj, date startDate);
		
		void		output(FILE*);

		void		convert(int _num, bool _inHRE, const vector<CK2Province*>& _srcProvinces, const vector<int>& _srcProvinceNums, const vector<EU3Country*>& _cores);
		void		addAdvisor(EU3Advisor* advisor);
		void		determineCulture(const cultureMapping& cultureMap, const vector<CK2Province*>& srcProvinces, const vector<CK2Barony*>& baronies);
		void		determineReligion(const religionMapping& religionMap, const vector<CK2Province*>& srcProvinces);
		void		setManpower(double _manpower);
		void		determinePopUnits();
		void		determineGoodsSupply(const tradeGoodMapping& tradeGoodMap);
		void		determineGoodsDemand(const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroupMap);
		void		addSupplyContribution(map<string, double>& goodsSupply);
		void		addDemandContribution(map<string, double>& goodsDemand);
		double	determineTax(const cultureGroupMapping& cultureGroups);
		double	determineManu();
		double	determineTolls();
		double	determineProduction(const map<string, double>& unitPrices);
		double	determineGold();
		void		setDiscoverers(map< string, vector<string> >& mapSpreadStrings);
		void		removeCore(EU3Country*);

		int		getNumRegiments()	{ return ++numRegiments; };
		int		getNumShips()		{ return ++numShips; };

		void		setCoastal(bool _coastal)					{ coastal = _coastal; };
		void		setBaseTax(double _baseTax)				{ baseTax = _baseTax; };
		void		setPopulation(double _population)		{ population = _population; };
		void		addCore(EU3Country* core)					{ cores.push_back(core); };
		void		clearCores()									{ cores.clear(); };
		void		setOwner(EU3Country* _owner)				{ owner = _owner; ownerStr = ""; };
		void		setSrcOwner(const CK2Title* _srcOwner)	{ srcOwner = _srcOwner; };
		void		addBuilding(const string building)		{ buildings.push_back(building); };
		void		setContinent(string _continent)			{ continent = _continent; };
		void		setSameContinent(bool _same)				{ sameContinent = _same; };
		void		setLandConnection(bool _connected)		{ landConnection = _connected; };
		void		setInHRE(bool _inHRE)						{ inHRE = _inHRE; };
		void		setReligion(string _religion)				{ religion = _religion; };
		void		setCot(bool _cot)								{ cot = _cot; };
		void		setArmyHere(bool _armyHere)				{ armyHere = _armyHere; };

		int						getNum() const				{ return num; };
		bool						isLand() const				{ return land; };
		bool						isCoastal() const			{ return coastal; };
		string					getCapital() const		{ return capital; };
		string					getTradeGood() const		{ return tradeGood; };
		EU3Country*				getOwner() const			{ return owner; };
		string					getOwnerStr() const		{ return ownerStr; };
		vector<string>			getCoreStrings() const	{ return coreStrings; };
		double					getBaseTax() const		{ return baseTax; };
		double					getPopulation() const	{ return population; };
		double					getManpower() const		{ return manpower; };
		vector<string>			getDiscoveredBy() const	{ return discoveredBy; };
		string					getCulture() const		{ return culture; };
		bool						hasCOT() const				{ return cot; }
		bool						hasTradeStation() const	{ return tradeStation; };
		vector<CK2Province*>	getSrcProvinces() const	{ return srcProvinces; };
	private:
		vector<string>			getCoreTags() const;

		vector<CK2Province*>		srcProvinces;
		vector<int>					srcProvinceNums;
		date							startDate;

		int							num;
		bool							land;
		bool							coastal;

		string						capital;
		string						tradeGood;
		double						baseTax;
		double						population;
		int							manpower;
		vector<string>				buildings;
		EU3Country*					owner;
		string						ownerStr;
		const CK2Title*			srcOwner;
		vector<string>				coreStrings;
		vector<EU3Country*>		cores;
		bool							inHRE;
		vector<string>				rawDiscoverers;
		vector<string>				discoveredBy;
		string						culture;
		string						religion;
		vector<EU3History*>		history;
		vector<string>				modifiers;
		bool							cot;
		bool							tradeStation;

		string	continent;
		bool		sameContinent;
		bool		landConnection;

		double					popUnits;
		double					supply;
		map<string, double>	demands;
		bool						armyHere;

		double	nativeSize;
		int		nativeFerocity;
		int		nativeHostility;

		int	numRegiments;
		int	numShips;
};



#endif	// EU3PROVINCE_H_