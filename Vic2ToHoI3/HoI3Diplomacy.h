#ifndef HOI3DIPLOMACY_H_
#define HOI3DIPLOMACY_H_


#include "Parsers/Object.h"
#include "Date.h"

struct HoI3Agreement
{
	string type;
	string country1;
	string country2;
	date start_date;
};

class HoI3Diplomacy
{
	public:
		void	addAgreement(HoI3Agreement agr) { agreements.push_back(agr); };
		void	output(FILE*);
		vector<HoI3Agreement>	getAgreements() const { return agreements; };
	private:
		vector<HoI3Agreement>	agreements;
};

#endif