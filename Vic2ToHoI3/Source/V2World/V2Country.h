/*Copyright (c) 2015 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_


#include "../CountryMapping.h"
#include "../Mapper.h"
#include "../Color.h"
#include "Date.h"
#include "V2Inventions.h"
#include "V2Localisation.h"
#include "V2Province.h"
#include <vector>
#include <map>
#include <set>
#include <string>
using namespace std;


class		V2Army;
class		V2Factory;
class		V2Leader;
struct	V2Party;
class		V2Province;
class		V2Reforms;
class		V2Relations;
class		V2World;


typedef struct V2State
{
	vector<int> provinces;
} V2State;



class V2Country
{
	public:
		V2Country(Object* obj, const inventionNumToName& iNumToName, map<string, string>& armyTechs, map<string, string>& navyTechs, const continentMapping& continentMap);

		void								addProvince(int num, V2Province* _province)		{ provinces.insert(make_pair(num, _province)); }
		void								setColor(Color newColor)					{ color = newColor; }
		void								setGreatNationRanking(int newRanking)	{ greatNationRanking = newRanking; }

		void								addCore(V2Province*);
		void								eatCountry(V2Country* target);
		void								clearProvinces();
		void								clearCores();

		map<string, V2Relations*>	getRelations()													const { return relations; }
		vector<V2State>				getStates()														const { return states; }
		map<int, V2Province*>		getProvinces()													const { return provinces; }
		vector<V2Province*>			getCores()														const { return cores; }
		string							getTag()															const { return tag; }
		string							getPrimaryCulture()											const { return primaryCulture; }
		vector<string>					getInventions()												const { return inventions; }
		int								getNumArmyTechs()												const { return numArmyTechs; }
		int								getNumNavyTechs()												const { return numNavyTechs; }
		string							getGovernment()												const { return government; }
		int								getCapital()													const { return capital; }
		string							getCapitalContinent()										const { return capitalContinent; }
		vector<string>					getTechs()														const { return techs; }
		Color								getColor()														const { return color; }
		string							getName()														const { return name; }
		string							getFlagFile()													const { return flagFile; }
		double							getEducationSpending()										const { return educationSpending; }
		double							getMilitarySpending()										const { return militarySpending; }
		unsigned	int					getRulingPartyId()											const { return rulingPartyId; }
		vector<unsigned int>			getActiveParties()											const { return activeParties; };
		vector<V2Army*>				getArmies()														const { return armies; }
		vector<V2Leader*>				getLeaders()													const { return leaders; }
		double							getRevanchism()												const { return revanchism; }
		double							getWarExhaustion()											const { return warExhaustion; }
		string							getTechSchool()												const { return techSchool; }
		map<string, string>			getAllReforms()												const { return reformsArray; }

		string							getReform(string reform) const;
		string							getName(const string& language) const;
		string							getAdjective(const string& language) const;
		double							getUpperHousePercentage(string ideology) const;

		void setLocalisationName(const string& language, const string& name)
		{
			if (this->name != "") // Domains have their name set from domain_region
			{
				namesByLanguage[language] = this->name;
			}
			else
			{
				namesByLanguage[language] = name;
				if (language == "english") this->name = name;
			}
		}
		void setLocalisationAdjective(const string& language, const string& adjective)
		{
			if (this->adjective != "") // Domains have their adjective set from domain_region
			{
				adjectivesByLanguage[language] = this->adjective;
			}
			else
			{
				adjectivesByLanguage[language] = adjective;
				if (language == "english") this->adjective = adjective;
			}
		}

	private:
		string							tag;
		vector<V2State>				states;
		map<int, V2Province*>		provinces;	// ID, province
		vector<V2Province*>			cores;
		int								capital;
		string							capitalContinent;
		string							primaryCulture;
		vector<string>					techs;
		vector<string>					inventions;
		int								numArmyTechs;
		int								numNavyTechs;
		string							government;
		map<string,V2Relations*>	relations;
		vector<V2Army*>				armies;
		vector<V2Leader*>				leaders;
		Color								color;	
		map<string, double>			upperHouseComposition;
		double							educationSpending;
		double							militarySpending;
		double							revanchism;
		double							warExhaustion;
		map<string, string>			reformsArray;
		string							flagFile;
		unsigned	int					rulingPartyId;
		vector<unsigned int>			activeParties;
		int								greatNationRanking;
		string							techSchool;

		// Localisation attributes
		string name;			// the name of this country
		string adjective;		// the adjective for this country
		map<string, string> namesByLanguage;		// the names of this country in different localisations
		map<string, string> adjectivesByLanguage;	// the adjectives for this country in different localisations
};



#endif	// V2COUNTRY_H_