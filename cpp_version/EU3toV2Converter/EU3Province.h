#ifndef EU3PROVINCE_H_
#define EU3PROVINCE_H_


#include "Parsers/Object.h"


class EU3Province {
	public:
		void				init(Object* obj);
		int				getNum();
		string			getOwner();
		vector<string>	getCores();
		string			getCulture();
		string			getReligion();
		bool				isColony();
	private:
		int				num;
		string			owner;
		//controller
		vector<string>	cores;
		string			culture;
		string			religion;
		int				population;
		bool				colony;
};


#endif // EU3PROVINCE_H_