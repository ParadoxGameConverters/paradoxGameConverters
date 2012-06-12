#ifndef EU3DIPLOMACY_H_
#define EU3DIPLOMACY_H_


#include "Date.h"

class Object;



struct EU3Agreement
{
	EU3Agreement();
	EU3Agreement(Object* obj);

	string	type;
	string	country1;
	string	country2;
	date		start_date;
};


class EU3Diplomacy
{
	public:
		EU3Diplomacy();
		void						init(Object* obj);
		vector<EU3Agreement>	getAgreements() { return agreements; };
	private:
		vector<EU3Agreement>	agreements;
};

#endif