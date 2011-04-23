#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_

#include <string>
#include "Parsers/Object.h"
#include "Log.h"
using namespace std;

class EU3Province {
	public:
		void		init(Object* obj);
		int		getNum();
		string	getOwner();
		string	getCulture();
		string	getReligion();
		bool		isColony();
	private:
		int		num;
		string	owner;
		//controller
		//cores
		string	culture;
		string	religion;
		int		population;
		bool		colony;
};


#endif // EU3PROVINCE_H_