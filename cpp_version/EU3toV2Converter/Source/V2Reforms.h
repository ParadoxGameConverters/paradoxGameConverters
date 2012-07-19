#ifndef V2REFORMS_H_
#define V2REFORMS_H_



#include <stdio.h>
#include <string>
using namespace std;

class EU3Country;
class V2Country;



class V2Reforms {
	public:
		V2Reforms(const EU3Country*);
		void output(FILE*) const;
	private:
		void governmentEffects(const EU3Country*);
		void sliderEffects(const EU3Country*);
		void flagEffects(const EU3Country*);
		void modifierEffects(const EU3Country*);

		int slavery;
		int vote_franchise;
		int upper_house_composition;
		int voting_system;
		int public_meetings;
		int press_rights;
		int trade_unions;
		int political_parties;
		int wage_reforms;
		int work_hours;
		int safety_regulations;
		int unemployment_subsidies;
		int pensions;
		int health_care;
		int school_reforms;
};


class V2UncivReforms {
	public:
		V2UncivReforms(int westernizationProgress, double milFocus, double socioEcoFocus, V2Country* country);
		void output(FILE*) const;
	private:
		bool reforms[16];
};


#endif V2REFORMS_H_