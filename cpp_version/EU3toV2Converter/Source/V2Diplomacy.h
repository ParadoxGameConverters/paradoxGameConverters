#ifndef V2DIPLOMACY_H_
#define V2DIPLOMACY_H_


#include "Parsers/Object.h"
#include "Date.h"

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