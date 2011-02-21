#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_

#include <stdio.h>
#include <string>
#include "Parsers/Object.h"
#include "tempFuncs.h"
using namespace std;

class V2Province {
	public:
		void		init(Object* obj);
		int		getNum();
		void		setOwner(string);
		string	getOwner();
		void		output(FILE*);
	private:
		bool		land;
		int		num;
		string	name;
		string	owner;
		//controller
		//cores
		//colonial
		int		oldPopulation;
		//pops
		string	rgoType;
		int		lifeRating;
};


#endif // V2PROVINCE_H_