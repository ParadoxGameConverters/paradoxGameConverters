#ifndef V2DIPLOMACY_H_
#define V2DIPLOMACY_H_


#include "Parsers/Object.h"
#include "Date.h"

struct V2Agreement
{
	V2Agreement();
	V2Agreement(Object* obj);

	string type;
	string country1;
	string country2;
	date start_date;
};

class V2Diplomacy
{
	public:
		void				init(Object* obj);
		vector<V2Agreement>	getAgreements() { return agreements; };
	private:
		vector<V2Agreement>	agreements;
};

#endif