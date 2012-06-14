#ifndef V2DIPLOMACY_H_
#define V2DIPLOMACY_H_



#include "Date.h"
#include <vector>

class Object;



struct V2Agreement
{
	string type;
	string country1;
	string country2;
	date start_date;
};


class V2Diplomacy
{
	public:
		void	addAgreement(V2Agreement agr) { agreements.push_back(agr); };
		void	output(FILE*);
	private:
		vector<V2Agreement>	agreements;
};



#endif