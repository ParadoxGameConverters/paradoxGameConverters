#ifndef HOI3COUNTRY_H_
#define HOI3COUNTRY_H_

#include <string>
#include <vector>
#include <map>
#include "Date.h"

#include "HoI3Relations.h"
#include "HoI3Army.h"
//#include "HoI3Creditor.h"
//#include "HoI3Leader.h"
#include "HoI3Alignment.h"

class HoI3Country
{
	public:
		void					init(string tag, string countryFile);
		void					initFromHistory();
		string					getTag() const;
		void					setSourceCountryIndex(int);
		int						getSourceCountryIndex();
		void					setCapital(int);
		int						getCapital() const					{ return capital; };
		void					output(FILE*);
		void					addRelations(HoI3Relations);
		HoI3Relations*			getRelations(string);
		void					addArmy(HoI3RegGroup);
		void					addTheatre(HoI3RegGroup);
		void					sortRelations(const vector<string>& order);
		void					setMoney(double);
		void					addLoan(string creditor, double size, double interest);
		void					setDiploPoints(double);
		//XXX: void					addLeader(HoI3Leader);
		void					setTechnology(string tech, int level);
		void					addProvince(int index);	// note: takes index into provinces array, NOT province num
		vector<int>				getProvinces() const;	// note: returns indexes into provinces array, NOT province nums
		HoI3Alignment*			getAlignment();
		void					setGovernment(string newGov)		{ government = newGov; };
		void					setLastElection(date electionDate)	{ lastElection = electionDate; };

		void					createArmyHQs(HoI3RegimentType hqBrigade);

		// laws
		void					setCivilLaw(string newLaw)			{ civil_law = newLaw; };
		void					setConscriptionLaw(string newLaw)	{ conscription_law = newLaw; };
		void					setEconomicLaw(string newLaw)		{ economic_law = newLaw; };
		void					setEducationalLaw(string newLaw)	{ educational_investment_law = newLaw; };
		void					setIndustrialLaw(string newLaw)		{ industrial_policy_laws = newLaw; };
		void					setPressLaw(string newLaw)			{ press_laws = newLaw; };
		void					setTrainingLaw(string newLaw)		{ training_laws = newLaw; };

	private:
		string				tag;
		int					capital;
		int					sourceCountryIndex;
		string				countryFile;
		vector<HoI3Relations>	relations;
		vector<HoI3RegGroup>	armies;
		double				money;
		//XXX: vector<HoI3Creditor>	creditors;
		double				diploPoints;
		//XXX: vector<HoI3Leader>	leaders;
		map<string, int>	technologies;
		vector<int>			provinces;
		HoI3Alignment		alignment;
		string				government;
		date				lastElection;

		// laws
		string				civil_law;
		string				conscription_law;
		string				economic_law;
		string				educational_investment_law;
		string				industrial_policy_laws;
		string				press_laws;
		string				training_laws;
};


#endif	// HoI3COUNTRY_H_
