#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_


#include "EU3World.h"
#include "V2Pop.h"
#include "EU3Country.h"
#include "Configuration.h"



struct V2Demographic {
	string culture;
	string religion;
	double ratio;
	double literacy;
	int oldProvinceID;
	string oldCountry;
};


class V2Province {
	public:
		void		init(int number);
		int		getNum();
		string	getName();
		void		setName(string);
		void		setOwner(string);
		string	getOwner();
		void		addCore(string);
		void		setColonial(bool);
		string	getCulture();
		void		setRgoType(string);
		string		getRgoType();
		void		setLifeRating(int);
		bool		isColonial();
		void		setColonised(bool);
		bool		wasColonised();
		void		setPaganConquest(bool);
		bool		wasPaganConquest();
		void		setCOT(bool);
		bool		getCOT();
		void		addOldPop(V2Pop);
		void		output(FILE*);
		vector<V2Pop>	getPops(string type);
		void		addPopDemographic(V2Demographic d);
		void		doCreatePops(bool isStateCapital, int statePopulation, EU3World& sourceWorld, bool stateHasCOT);
		void		setPopConMil(string nationalCulture, vector<string> acceptedCultures, string nationalReligion, double nationalConModifier, double nationalMilModifier);
		void		setCoastal(bool coastal);
		bool		isCoastal();
		bool		isLand();
		int			getTotalPopulation() const;
		int			getOldPopulation() const;
		int			getSoldierPopForArmy(bool force = false);
		pair<int, int> getAvailableSoldierCapacity() const;
		string		getRegimentName(RegimentCategory rc);
		void		setFortLevel(int);
		void		setNavalBaseLevel(int);
	private:
		void		createPops(const V2Demographic& d, bool isStateCapital, int statePopulation, EU3Province* oldProvince, EU3Country* oldCountry, bool stateHasCOT);
		void		combinePops();
		bool		growSoldierPop(int popID);

		bool				land;
		bool				coastal;
		int				num;
		string			name;
		string			owner;
		//controller
		vector<string>	cores;
		bool				colonial;
		bool				colonised;
		bool				COT;
		bool				originallyPagan;
		int				oldPopulation;
		vector<V2Demographic> demographics;
		vector<V2Pop>	oldPops;
		vector<V2Pop>	pops;
		string			rgoType;
		int				lifeRating;
		int				unitNameCount[num_reg_categories];
		int				fortLevel;
		int				navalBaseLevel;
};


#endif // V2PROVINCE_H_