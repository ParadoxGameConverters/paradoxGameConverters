#ifndef EU3DIPLOMACY_H_
#define EU3DIPLOMACY_H_


#include "..\Date.h"
#include <vector>
using namespace std;



class Object;
class EU3Country;



struct EU3Agreement
{
	EU3Agreement();
	void output(FILE*);

	string		type;
	EU3Country*	country1;
	EU3Country*	country2;
	date			startDate;
};


class EU3Diplomacy
{
	public:
		EU3Diplomacy();
		void output(FILE*);
		void addAgreement(EU3Agreement* agr) { agreements.push_back(agr); }
		void removeAgreement(EU3Agreement* agr);
	private:
		vector<EU3Agreement*>	agreements;
};

// Types of agreements:
//open_market - care
//casus_belli - care
//royal_marriage - care
//guarantee - care
//alliance - care
//union - care
//sphere - care
//vassal - care

#endif