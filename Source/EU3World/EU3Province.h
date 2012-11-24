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
		EU3Province(int _num, Object* obj, date startDate, map< string, vector<string> >& mapSpreadStrings);
		
		void		output(FILE*);

		void		convert(int _num, bool _inHRE, vector<string> _discoveredBy, const vector<CK2Province*>& _srcProvinces, const vector<string> _cores);
		void		addAdvisor(EU3Advisor* advisor);
		void		determineCulture(const cultureMapping& cultureMap, const vector<CK2Province*>& srcProvinces, const vector<CK2Barony*> baronies);
		void		determineReligion(const religionMapping& religionMap, const vector<CK2Province*>& srcProvinces);
		void		setManpower(double _manpower);
		void		determinePopUnits();
		void		determineGoodsSupply(const tradeGoodMapping& tradeGoodMap, const EU3Country* country);
		void		determineGoodsDemand(const tradeGoodMapping& tradeGoodMap, const EU3Country* country, const religionGroupMapping& EU3ReligionGroupMap);
		void		addSupplyContribution(map<string, double>& goodsSupply);
		void		addDemandContribution(map<string, double>& goodsDemand);
		double	determineTax(EU3Country* country, const cultureGroupMapping& cultureGroups);
		double	determineTolls(EU3Country* country);
		double	determineProduction(EU3Country* country, const map<string, double>& unitPrices);
		double	determineGold();

		void		setBaseTax(double _baseTax)				{ baseTax= _baseTax; };
		void		setPopulation(double _population)		{ population = _population; };
		void		addCore(string core)							{ cores.push_back(core); };
		void		setOwner(string _owner)						{ owner = _owner; };
		void		setSrcOwner(const CK2Title* _srcOwner)	{ srcOwner = _srcOwner; };
		void		setContinent(string _continent)			{ continent = _continent; };
		void		setSameContinent(bool _same)				{ sameContinent = _same; };
		void		setLandConnection(bool _connected)		{ landConnection = _connected; };
		void		setInHRE(bool _inHRE)						{ inHRE = _inHRE; };

		bool						isLand() const				{ return land; };
		string					getOwner() const			{ return owner; };
		double					getBaseTax() const		{ return baseTax; };
		double					getPopulation() const	{ return population; };
		double					getManpower() const		{ return manpower; };
		string					getCulture() const		{ return culture; };
		bool						hasCOT() const				{ return false; }
		vector<CK2Province*>	getSrcProvinces() const	{ return srcProvinces; };
	private:
		vector<CK2Province*>		srcProvinces;

		int							num;
		bool							land;

		string						capital;
		string						tradeGood;
		double						baseTax;
		double						population;
		int							manpower;
		string						owner;
		const CK2Title*			srcOwner;
		vector<string>				cores;
		bool							inHRE;
		vector<string>				discoveredBy;
		string						culture;
		string						religion;
		vector<EU3History*>		history;
		vector<string>				modifiers;

		string	continent;
		bool		sameContinent;
		bool		landConnection;

		double					popUnits;
		double					supply;
		map<string, double>	demands;
};



#endif	// EU3PROVINCE_H_