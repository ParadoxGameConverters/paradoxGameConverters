#ifndef V2REFORMS_H_
#define V2REFORMS_H_


#include "EU3Country.h"


class V2Reforms {
	public:
		void init(EU3Country*);
		void governmentEffects(EU3Country*);
		void sliderEffects(EU3Country*);
		void flagEffects(EU3Country*);
		void modifierEffects(EU3Country*);
		void output(FILE*);
	private:
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




#endif V2REFORMS_H_