#ifndef EU3DIPLOMACY_H_
#define EU3DIPLOMACY_H_


#include "Date.h"
#include <vector>
using namespace std;

class Object;



struct EU3Agreement
{
	EU3Agreement(Object* obj);

	string	type;
	string	country1;
	string	country2;
	date		startDate;
};


class EU3Diplomacy
{
	public:
		EU3Diplomacy();
		EU3Diplomacy(Object* obj);
		vector<EU3Agreement>	getAgreements() const { return agreements; };
	private:
		vector<EU3Agreement>	agreements;
};

#endif