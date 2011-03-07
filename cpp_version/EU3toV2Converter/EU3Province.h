#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_

#include <string>
#include "Parsers/Object.h"
using namespace std;

class EU3Province {
	public:
		void		init(Object* obj);
		int		getNum();
		string	getOwner();
		bool		isColony();
	private:
		int		num;
		string	owner;
		//controller
		//cores
		//culture
		//religion
		int		population;
		bool		colony;
};


#endif // EU3PROVINCE_H_