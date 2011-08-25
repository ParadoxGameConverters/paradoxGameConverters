#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_


#include "V2State.h"
#include "V2Inventions.h"
#include "V2Relations.h"
#include "V2Army.h"
#include "V2Reforms.h"


class V2Country
{
	public:
		void					init(string tag, string countryFile, vector<int> parties);
		string				getTag() const;
		void					setSourceCountryIndex(int);
		void					addState(V2State);
		void					setCapital(int);
		void					setcivilized(bool);
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
		inventionStatus	getInventionState(inventionTypes);
		void					addPrestige(double);
		void					setGovernment(string);
		void					addRelations(V2Relations);
		V2Relations*			getRelations(string);
		void					addArmy(V2Army);
		V2Army*					getArmyForRemainder(RegimentCategory rc);
		void					setReforms(EU3Country*);
		void					setNationalIdea(EU3Country*, int& libertyLeft, int& equalityLeft);
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
		int					armyTech;
		int					navyTech;
		int					commerceTech;
		int					industryTech;
		int					cultureTech;
		inventionStatus	inventions[naval_exercises];
		string				government;
		vector<V2Relations>	relations;
		vector<V2Army>		armies;
		V2Reforms			reforms;
		string				nationalValue;
};


#endif	// V2COUNTRY_H_