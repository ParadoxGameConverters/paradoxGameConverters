#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_


#include "V2State.h"
#include "V2Inventions.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Reforms.h"
#include "V2Factory.h"
#include "V2Creditor.h"
#include "V2Leader.h"


class V2Country
{
	public:
		void					init(string tag, string countryFile, vector<int> parties);
		void					initFromHistory();
		string				getTag() const;
		void					setSourceCountryIndex(int);
		void					addState(V2State);
		void					setCapital(int);
		void					setCivilized(bool);
		bool					isCivilized();
		void					setPrimaryCulture(string);
		string				getPrimaryCulture() const;
		void					addAcceptedCulture(string);
		void					setReligion(string);
		string				getReligion();
		void					output(FILE*);
		int					getSourceCountryIndex();
		void					setPrestige(double);
		void					setArmyTech(double);
		void					setNavyTech(double);
		void					setCommerceTech(double);
		void					setIndustryTech(double);
		void					setCultureTech(double);
		inventionStatus	getInventionState(inventionType);
		void					addPrestige(double);
		void					addPlurality(double);
		void					setGovernment(string);
		void					addRelations(V2Relations);
		V2Relations*			getRelations(string);
		void					addArmy(V2Army);
		V2Army*					getArmyForRemainder(RegimentCategory rc);
		void					setReforms(EU3Country*);
		void					setNationalIdea(EU3Country*, int& libertyLeft, int& equalityLeft);
		void					sortRelations(const vector<string>& order);
		bool					addFactory(V2Factory);
		void					setMoney(double);
		void					setLastBankrupt(date);
		void					addLoan(string creditor, double size, double interest);
		void					setBankReserves(double);
		void					setDiploPoints(double);
		void					setBadboy(double);
		void					addLeader(V2Leader);
		void					setupPops(EU3World& sourceWorld);
		void					setLiteracy(double);
		double				getLiteracy();

	private:
		void outputTech(FILE*);
		void outputInventions(FILE*);

		string				tag;
		vector<V2State>	states;
		int					capital;
		bool					civilized;
		string				primaryCulture;
		vector<string>		acceptedCultures;
		string				religion;
		vector<int>			parties;
		int					sourceCountryIndex;
		string				countryFile;
		double				prestige;
		double				plurality;
		vector<string>		techs;
		inventionStatus	inventions[naval_exercises];
		string				government;
		vector<V2Relations>	relations;
		vector<V2Army>		armies;
		V2Reforms			reforms;
		string				nationalValue;
		double				money;
		date				lastBankrupt;
		vector<V2Creditor>	creditors;
		double				bankReserves;
		double				diploPoints;
		double				badboy;
		vector<V2Leader>	leaders;
		double				literacy;
};


#endif	// V2COUNTRY_H_