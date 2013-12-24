#ifndef EU4DIPLOMACY_H_
#define EU4DIPLOMACY_H_


#include "Date.h"
#include <vector>
using namespace std;

class Object;



struct EU4Agreement
{
	EU4Agreement(Object* obj);

	string	type;
	string	country1;
	string	country2;
	date		startDate;
};


class EU4Diplomacy
{
	public:
		EU4Diplomacy();
		EU4Diplomacy(Object* obj);
		vector<EU4Agreement>	getAgreements() const { return agreements; };
	private:
		vector<EU4Agreement>	agreements;
};

#endif