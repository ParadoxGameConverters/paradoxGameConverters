#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_



#include "../Configuration.h"
#include "../EU4World/EU4World.h"
#include "../EU4World/EU4Country.h"

class V2Pop;
class V2Factory;



struct V2Demographic
{
	string								culture;
	string								religion;
	double								ratio;
	EU4Province*						oldProvince;
	EU4Country*							oldCountry;
};


class V2Province
{
	public:
		V2Province(string _filename);
		void output() const;
		void convertFromOldProvince(const EU4Province* oldProvince);
		void determineColonial();
		void addCore(string);
		void addOldPop(const V2Pop*);
		void doCreatePops(bool isStateCapital, int statePopulation);
		void sortPops();

		int				getTotalPopulation() const;
		vector<V2Pop*>	getPops(string type) const;
		int				getSoldierPopForArmy(bool force = false);
		pair<int, int>	getAvailableSoldierCapacity() const;
		string			getRegimentName(RegimentCategory rc);
		bool				hasCulture(string culture, float percentOfPopulation) const;
		
		void				setCoastal(bool _coastal)				{ coastal = _coastal; };
		void				setName(string _name)					{ name = _name; };
		void				setOwner(string _owner)					{ owner = _owner; };
		void				setLandConnection(bool _connection)	{ landConnection = _connection; };
		void				setSameContinent(bool _same)			{ sameContinent = _same; };
		void				addPopDemographic(V2Demographic d)	{ demographics.push_back(d); };
		void				setFortLevel(int level)					{ fortLevel = level; };
		void				setNavalBaseLevel(int level)			{ navalBaseLevel = level; };
		void				setRailLevel(int level)					{ railLevel = level; };

		const EU4Province*	getSrcProvince()		const { return srcProvince; };
		int						getOldPopulation()	const	{ return oldPopulation; };
		bool						wasInfidelConquest()	const { return originallyInfidel; };
		bool						wasColonised()			const { return colonised; };
		bool						isColonial()			const { return colonial != 0; };
		string					getRgoType()			const { return rgoType; };
		string					getOwner()				const { return owner; };
		int						getNum()					const { return num; };
		string					getName()				const { return name; };
		bool						isCoastal()				const { return coastal; };
		bool						hasNavalBase()			const { return (navalBaseLevel > 0); };
	private:
		void outputPops(FILE*) const;
		void outputUnits(FILE*) const;
		void createPops(const V2Demographic& d, bool isStateCapital, int statePopulation);
		void combinePops();
		bool growSoldierPop(V2Pop* pop);

		const EU4Province*			srcProvince;
		
		string							filename;
		bool								coastal;
		int								num;
		string							name;
		string							owner;
		vector<string>					cores;
		int								colonyLevel;
		int								colonial;
		bool								colonised;
		bool								landConnection;
		bool								sameContinent;
		bool								originallyInfidel;
		int								oldPopulation;
		vector<V2Demographic>		demographics;
		vector<const V2Pop*>			oldPops;
		vector<V2Pop*>					pops;
		string							rgoType;
		string							terrain;
		int								lifeRating;
		bool								slaveState;
		int								unitNameCount[num_reg_categories];
		int								fortLevel;
		int								navalBaseLevel;
		int								railLevel;
		vector<const V2Factory*>	factories;
};



#endif // V2PROVINCE_H_