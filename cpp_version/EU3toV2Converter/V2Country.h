#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_


#include "V2State.h"
#include "V2Inventions.h"
#include "V2Relations.h"


class V2Country
{
	public:
		void					init(string tag, string countryFile, vector<int> parties);
		string				getTag();
		void					setSourceCountryIndex(int);
		void					addState(V2State);
		void					setCapital(int);
		void					setcivilized(bool);
		void					setPrimaryCulture(string);
		void					addAcceptedCulture(string);
		void					setReligion(string);
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
};


#endif	// V2COUNTRY_H_