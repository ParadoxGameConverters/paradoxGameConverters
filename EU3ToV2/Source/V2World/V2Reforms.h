#ifndef V2REFORMS_H_
#define V2REFORMS_H_



#include <stdio.h>
#include <string>
using namespace std;

class EU3Country;
class V2Country;



class V2Reforms {
	public:
		V2Reforms(const V2Country*, const EU3Country*);
		void output(FILE*) const;
	private:
		void governmentEffects(const V2Country* dstCountry);
		void upperHouseEffects(const V2Country* dstCountry);

		double slavery;
		double vote_franchise;
		double upper_house_composition;
		double voting_system;
		double public_meetings;
		double press_rights;
		double trade_unions;
		double political_parties;
};


class V2UncivReforms {
	public:
		V2UncivReforms(int westernizationProgress, double milFocus, double socioEcoFocus, V2Country* country);
		void output(FILE*) const;
	private:
		bool reforms[16];
};


#endif V2REFORMS_H_