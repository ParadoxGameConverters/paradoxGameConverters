#ifndef V2PROVINCE_H_
#define V2PROVINCE_H_

#include <string>
#include "Parsers/Object.h"
using namespace std;

class V2Province {
	public:
		void		init(Object* obj);
		int		getNum();
		void		setOwner(string);
		string	getOwner();
	private:
		int		num;
		string	name;
		string	owner;
		//controller
		//cores
		//garrison
		//colonial
		//pops
		//rgo
		//life_rating
		//last_imigration
		//crime (can be left off)
};


#endif // V2PROVINCE_H_