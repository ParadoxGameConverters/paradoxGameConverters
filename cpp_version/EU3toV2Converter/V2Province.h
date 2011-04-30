#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_

#include <stdio.h>
#include <string>
#include "Parsers/Object.h"
#include "tempFuncs.h"
#include "V2Pop.h"
#include "EU3Province.h"
#include "EU3Country.h"
using namespace std;

class V2Province {
	public:
		void		init(Object* obj);
		int		getNum();
		string	getName();
		void		setOwner(string);
		string	getOwner();
		void		setColonial(bool);
		void		setCulture(string);
		void		setReligion(string);
		bool		isColonial();
		void		createPops(EU3Province* oldProvince, EU3Country* oldCountry);
		void		output(FILE*);
	private:
		bool		land;
		int		num;
		string	name;
		string	owner;
		//controller
		//cores
		bool				colonial;
		int				oldPopulation;
		vector<V2Pop>	pops;
		string			culture;
		string			religion;
		string			rgoType;
		int				lifeRating;
};


#endif // V2PROVINCE_H_