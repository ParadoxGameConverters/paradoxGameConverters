#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_

#include "Date.h"

#include "V2Province.h"
#include "V2Inventions.h"
#include "V2Relations.h"
#include "V2Army.h"
// #include "V2Reforms.h"
// #include "V2Creditor.h"
// #include "V2Leader.h"


class V2Country
{
	public:
		void				init(Object* obj);
		string				getTag() const;
		void				addProvince(V2Province*);
		void				clearProvinces();
		void				addCore(V2Province*);
		void				clearCores();
		vector<V2Province*>	getProvinces();
		vector<V2Province*>	getCores();
		int					getCapital();
		void				eatCountry(V2Country* target);
		inventionStatus*	getInventionStatusArray() { return inventions; };
		vector<string>		getTechs() { return techs; };
		vector<V2Army>		getArmies() { return armies; };
		vector<V2Relations>	getRelations() { return relations; };
		string				getPrimaryCulture() { return primaryCulture; };
		double				getMoney() { return money; };
		double				getDiploPoints() { return diploPoints; };
		string				getGovernmentType() { return government; };
		int					getRulingParty() { return rulingParty; };
		double				getUpperHousePercentage(string ideology);
		double				getEducationSpending() { return educationSpending; }
		double				getAdminSpending() { return adminSpending; }
		double				getSocialSpending() { return socialSpending; }
		double				getMilitarySpending() { return militarySpending; }
		string				getReform(string reform);

	private:
		string				tag;
		int					capital;
		bool				civilized;
		string				primaryCulture;
		vector<string>		acceptedCultures;
		string				religion;
		vector<int>			parties;
		int					sourceCountryIndex;
		string				countryFile;
		double				prestige;
		double				plurality;
		vector<string>		techs;
		inventionStatus		inventions[naval_exercises];
		string				government;
		vector<V2Relations>	relations;
		vector<V2Army>		armies;
		//XXX: V2Reforms			reforms;
		//string				nationalValue;
		double				money;
		//XXX: vector<V2Creditor>	creditors;
		double				bankReserves;
		double				diploPoints;
		double				badboy;
		//XXX: vector<V2Leader>	leaders;
		double				literacy;
		vector<V2Province*>	provinces;
		vector<V2Province*>	cores;
		int					rulingParty;
		map<string, double>	upperHouseComposition;
		double				educationSpending;
		double				adminSpending;
		double				socialSpending;
		double				militarySpending;

		map<string, string> reforms;
};


#endif	// V2COUNTRY_H_