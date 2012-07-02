#ifndef V2DIPLOMACY_H_
#define V2DIPLOMACY_H_



#include "Date.h"
#include <vector>



struct V2Agreement
{
	string	type;
	string	country1;
	string	country2;
	date		start_date;
};


class V2Diplomacy
{
	public:
		V2Diplomacy() {agreements.clear(); };
		void output(FILE*);

		void addAgreement(V2Agreement agr) { agreements.push_back(agr); };
	private:
		vector<V2Agreement>	agreements;
};



#endif