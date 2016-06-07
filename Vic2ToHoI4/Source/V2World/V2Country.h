/*Copyright (c) 2016 The Paradox Game Converters Project

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
#include "Vic2State.h"
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



class V2Country
{
	public:
		V2Country(Object* obj, const inventionNumToName& iNumToName, map<wstring, wstring>& armyTechs, map<wstring, wstring>& navyTechs, const continentMapping& continentMap);

		void								addProvince(int num, V2Province* _province)		{ provinces.insert(make_pair(num, _province)); }
		void								setColor(Color newColor)								{ color = newColor; }
		void								setGreatNation(bool _greatNation)					{ greatNation = _greatNation; }

		void								addCore(V2Province*);
		void								eatCountry(V2Country* target);
		void								clearProvinces();
		void								clearCores();
		void								putWorkersInProvinces();

		map<wstring, V2Relations*>	getRelations()													const { return relations; }
		vector<Vic2State>				getStates()														const { return states; }
		map<int, V2Province*>		getProvinces()													const { return provinces; }
		vector<V2Province*>			getCores()														const { return cores; }
		wstring							getTag()															const { return tag; }
		wstring							getPrimaryCulture()											const { return primaryCulture; }
		vector<wstring>				getInventions()												const { return inventions; }
		int								getNumArmyTechs()												const { return numArmyTechs; }
		int								getNumNavyTechs()												const { return numNavyTechs; }
		wstring							getGovernment()												const { return government; }
		int								getCapital()													const { return capital; }
		wstring							getCapitalContinent()										const { return capitalContinent; }
		vector<wstring>				getTechs()														const { return techs; }
		Color								getColor()														const { return color; }
		wstring							getName()														const { return name; }
		wstring							getFlagFile()													const { return flagFile; }
		double							getEducationSpending()										const { return educationSpending; }
		double							getMilitarySpending()										const { return militarySpending; }
		unsigned	int					getRulingPartyId()											const { return rulingPartyId; }
		vector<unsigned int>			getActiveParties()											const { return activeParties; };
		vector<V2Army*>				getArmies()														const { return armies; }
		vector<V2Leader*>				getLeaders()													const { return leaders; }
		double							getRevanchism()												const { return revanchism; }
		double							getWarExhaustion()											const { return warExhaustion; }
		wstring							getTechSchool()												const { return techSchool; }
		map<wstring, wstring>		getAllReforms()												const { return reformsArray; }
		bool								getGreatNation()												const { return greatNation; }

		wstring							getReform(wstring reform) const;
		wstring							getName(const wstring& language) const;
		wstring							getAdjective(const wstring& language) const;
		double							getUpperHousePercentage(wstring ideology) const;

		void setLocalisationName(const wstring& language, const wstring& name)
		{
			if (this->name != L"") // Domains have their name set from domain_region
			{
				namesByLanguage[language] = this->name;
			}
			else
			{
				namesByLanguage[language] = name;
				if (language == L"english") this->name = name;
			}
		}
		void setLocalisationAdjective(const wstring& language, const wstring& adjective)
		{
			if (this->adjective != L"") // Domains have their adjective set from domain_region
			{
				adjectivesByLanguage[language] = this->adjective;
			}
			else
			{
				adjectivesByLanguage[language] = adjective;
				if (language == L"english") this->adjective = adjective;
			}
		}

	private:
		wstring							tag;
		vector<Vic2State>				states;
		map<int, V2Province*>		provinces;	// ID, province
		vector<V2Province*>			cores;
		int								capital;
		wstring							capitalContinent;
		wstring							primaryCulture;
		vector<wstring>				techs;
		vector<wstring>				inventions;
		int								numArmyTechs;
		int								numNavyTechs;
		wstring							government;
		map<wstring,V2Relations*>	relations;
		vector<V2Army*>				armies;
		vector<V2Leader*>				leaders;
		Color								color;	
		map<wstring, double>			upperHouseComposition;
		double							educationSpending;
		double							militarySpending;
		double							revanchism;
		double							warExhaustion;
		map<wstring, wstring>		reformsArray;
		wstring							flagFile;
		unsigned	int					rulingPartyId;
		vector<unsigned int>			activeParties;
		bool								greatNation;
		wstring							techSchool;

		// Localisation attributes
		wstring name;											// the name of this country
		wstring adjective;									// the adjective for this country
		map<wstring, wstring> namesByLanguage;			// the names of this country in different localisations
		map<wstring, wstring> adjectivesByLanguage;	// the adjectives for this country in different localisations
};



#endif	// V2COUNTRY_H_