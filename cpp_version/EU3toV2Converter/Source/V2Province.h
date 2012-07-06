#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_



#include "EU3World.h"
#include "EU3Country.h"
#include "Configuration.h"

class V2Pop;



struct V2Demographic
{
	string								culture;
	string								religion;
	double								ratio;
	double								literacy;
	double								reactionary;
	double								conservative;
	double								liberal;
	EU3Province*						oldProvince;
	EU3Country*							oldCountry;
	vector< pair<int, double> >	issues;
};


class V2Province
{
	public:
		V2Province(int number);
		void output(FILE*) const;
		void importHistory(Object*);
		void convertFromOldProvince(EU3Province* oldProvince);
		void addCore(string);
		void addOldPop(const V2Pop*);
		void doCreatePops(WorldType game, bool isStateCapital, int statePopulation, bool stateHasCOT);
		void sortPops();

		void				setPopConMil(string nationalCulture, vector<string> acceptedCultures, string nationalReligion, double nationalConModifier, double nationalMilModifier);
		int				getTotalPopulation() const;
		vector<V2Pop*>	getPops(string type) const;
		int				getSoldierPopForArmy(bool force = false);
		pair<int, int>	getAvailableSoldierCapacity() const;
		string			getRegimentName(RegimentCategory rc);
		bool				hasCulture(string culture, float percentOfPopulation) const;
		
		void				setCoastal(bool _coastal)				{ coastal = _coastal; };
		void				setName(string _name)					{ name = _name; };
		void				setOwner(string _owner)					{ owner = _owner; };
		//void				setController(string _controller)	{ controller = _controller; };
		void				addPopDemographic(V2Demographic d)	{ demographics.push_back(d); };
		void				setFortLevel(int level)					{ fortLevel = level; };
		void				setNavalBaseLevel(int level)			{ navalBaseLevel = level; };
		void				setRailLevel(int level)					{ railLevel = level; };

		bool				isLand()					const { return land; };
		int				getOldPopulation()	const	{ return oldPopulation; };
		bool				getCOT()					const { return COT; };
		bool				wasPaganConquest()	const { return originallyPagan; };
		bool				wasColonised()			const { return colonised; };
		bool				isColonial()			const { return colonial; };
		string			getRgoType()			const { return rgoType; };
		string			getOwner()				const { return owner; };
		int				getNum()					const { return num; };
		string			getName()				const { return name; };
		bool				isCoastal()				const { return coastal; };
	private:
		void outputPops(FILE*) const;
		void outputRGO(FILE*) const;
		void outputUnits(FILE*) const;
		void createPops(WorldType game, const V2Demographic& d, bool isStateCapital, int statePopulation, bool stateHasCOT);
		void combinePops();
		bool growSoldierPop(V2Pop* pop);

		bool							land;
		bool							coastal;
		int							num;
		string						name;
		string						owner;
		//controller
		vector<string>				cores;
		bool							colonial;
		bool							colonised;
		bool							COT;
		bool							originallyPagan;
		int							oldPopulation;
		vector<V2Demographic>	demographics;
		vector<const V2Pop*>		oldPops;
		vector<V2Pop*>				pops;
		string						rgoType;
		int							lifeRating;
		int							unitNameCount[num_reg_categories];
		int							fortLevel;
		int							navalBaseLevel;
		int							railLevel;
};



#endif // V2PROVINCE_H_