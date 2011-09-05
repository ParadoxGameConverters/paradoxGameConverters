#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_


#include "V2Pop.h"
#include "EU3Country.h"


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
		void		addOldPop(V2Pop);
		void		createPops(string culture, string religion, double ratio, EU3Province* oldProvince, EU3Country* oldCountry);
		void		output(FILE*);
		vector<V2Pop>	getPops(string type);
		void		setCoastal(bool coastal);
		bool		isCoastal();
		int			getTotalPopulation() const;
		int			getSoldierPopForArmy(bool force = false);
		pair<int, int> getAvailableSoldierCapacity() const;
		string		getRegimentName(RegimentCategory rc);
		void		combinePops();
		void		setFortLevel(int);
		void		setNavalBaseLevel(int);
	private:
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
		int				oldPopulation;
		vector<V2Pop>	oldPops;
		vector<V2Pop>	pops;
		string			rgoType;
		int				lifeRating;
		int				unitNameCount[num_reg_categories];
		int				fortLevel;
		int				navalBaseLevel;
};


#endif // V2PROVINCE_H_