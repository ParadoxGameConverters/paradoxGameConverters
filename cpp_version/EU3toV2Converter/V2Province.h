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
		void		setCulture(string);
		string	getCulture();
		void		setReligion(string);
		void		setRgoType(string);
		void		setLifeRating(int);
		bool		isColonial();
		void		addOldPop(V2Pop);
		void		createPops(EU3Province* oldProvince, EU3Country* oldCountry);
		void		output(FILE*);
		vector<V2Pop>	getPops(string type);
		void		setCoastal(bool coastal);
		bool		isCoastal();
		int			getTotalPopulation();
		bool		growSoldierPop(int popID);
	private:
		bool				land;
		bool				coastal;
		int				num;
		string			name;
		string			owner;
		//controller
		vector<string>	cores;
		bool				colonial;
		int				oldPopulation;
		vector<V2Pop>	oldPops;
		vector<V2Pop>	pops;
		string			culture;
		string			religion;
		string			rgoType;
		int				lifeRating;
};


#endif // V2PROVINCE_H_