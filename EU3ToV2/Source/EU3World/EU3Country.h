/*Copyright (c) 2014 The Paradox Game Converters Project

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


#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <set>
#include "EU3Army.h"
#include "../Color.h"
#include "../Date.h"

class EU3Province;
class EU3Relations;
class EU3Loan;
class EU3Leader;



class EU3Country
{
	public:
		EU3Country(Object* obj);

		// Add any additional information available from the specified country file.
		void readFromCommonCountry(const string& fileName, Object*);

		void setLocalisationName(const unsigned int language, const string& name);
		void setLocalisationAdjective(const unsigned int language, const string& adjective);

		void							addProvince(EU3Province*);
		void							addCore(EU3Province*);
		bool							hasModifier(string) const;
		bool							hasNationalIdea(string) const;
		bool							hasFlag(string) const ;
		void							resolveRegimentTypes(const RegimentTypeMap& map);
		int							getManufactoryCount() const;
		double						inflationAdjust(double input) const;
		double						getBadboyLimit() const;
		void							eatCountry(EU3Country* target);

		string						getTag()										const { return tag; };
		vector<EU3Province*>		getProvinces()								const { return provinces; };
		vector<EU3Province*>		getCores()									const { return cores; };
		int							getCapital()								const { return capital; };
		int							getNationalFocus()						const { return nationalFocus; };
		string						getTechGroup()								const { return techGroup; };
		string						getPrimaryCulture()						const { return primaryCulture; };
		vector<string>				getAcceptedCultures()					const { return acceptedCultures; };
		string						getReligion()								const { return religion; };
		double						getPrestige()								const { return prestige; };
		double						getCulture()								const { return culture; };
		double						getArmyTradition()						const { return armyTradition; };
		double						getNavyTradition()						const { return navyTradition; };
		double						getStability()								const { return stability; };
		double						getLandTech()								const { return landTech; };
		double						getNavalTech()								const { return navalTech; };
		double						getTradeTech()								const { return tradeTech; };
		double						getProductionTech()						const { return productionTech; };
		double						getGovernmentTech()						const { return governmentTech; };
		double						getEstimatedMonthlyIncome()			const { return estMonthlyIncome; };
		double						getArmyInvestment()						const { return armyInvestment; };
		double						getNavyInvestment()						const { return navyInvestment; };
		double						getCommerceInvestment()					const { return commerceInvestment; };
		double						getIndustryInvestment()					const { return industryInvestment; };
		double						getCultureInvestment()					const { return cultureInvestment; };
		bool							getPossibleDaimyo()						const { return possibleDaimyo; };
		string						getGovernment()							const { return government; };
		vector<EU3Relations*>	getRelations()								const { return relations; };
		vector<EU3Army*>			getArmies()									const { return armies; };
		int							getCentralizationDecentralization()	const { return centralization_decentralization; };
		int							getAristocracyPlutocracy()				const { return aristocracy_plutocracy; };
		int							getSerfdomFreesubjects()				const { return serfdom_freesubjects; };
		int							getInnovativeNarrowminded()			const { return innovative_narrowminded; };
		int							getMercantilismFreetrade()				const { return mercantilism_freetrade; };
		int							getOffensiveDefensive()					const { return offensive_defensive; };
		int							getLandNaval()								const { return land_naval; };
		int							getQualityQuantity()						const { return quality_quantity; };
		date							getLastBankrupt()							const { return last_bankrupt; };
		vector<EU3Loan*>			getLoans()									const { return loans; };
		double						getDiplomats()								const { return diplomats; };
		double						getBadboy()									const { return badboy; };
		vector<EU3Leader*>		getLeaders()								const { return leaders; };

		double						getTreasury()								const	{ return inflationAdjust(treasury); };

		string	getName() const { return name; }
		string	getName(const unsigned int language) const;
		string	getAdjective(const unsigned int language) const;
		Color		getColor() const { return color; }

	private:
		void							checkIdea(const Object*, const string);
		void							clearProvinces();
		void							clearCores();

		string						tag;
		vector<EU3Province*>		provinces;
		vector<EU3Province*>		cores;
		int							capital;
		int							nationalFocus;
		string						techGroup;
		string						primaryCulture;
		vector<string>				acceptedCultures;
		string						religion;
		double						prestige;
		double						culture;
		double						armyTradition;
		double						navyTradition;
		double						stability;
		double						landTech;
		double						navalTech;
		double						tradeTech;
		double						productionTech;
		double						governmentTech;
		double						estMonthlyIncome;
		double						armyInvestment;
		double						navyInvestment;
		double						commerceInvestment;
		double						industryInvestment;
		double						cultureInvestment;
		map<string, bool>			flags;
		map<string, bool>			modifiers;
		bool							possibleDaimyo;
		vector<EU3Leader*>		leaders;
		string						government;
		vector<EU3Relations*>	relations;
		vector<EU3Army*>			armies; // and navies...
		int							centralization_decentralization;
		int							aristocracy_plutocracy;
		int							serfdom_freesubjects;
		int							innovative_narrowminded;
		int							mercantilism_freetrade;
		int							offensive_defensive;
		int							land_naval;
		int							quality_quantity;
		set<string>					nationalIdeas;
		double						treasury;
		date							last_bankrupt;
		vector<EU3Loan*>			loans;
		double						diplomats;
		double						badboy;

		double						legitimacy;
		double						inflation;

		string	name;
		string	adjective;
		Color		color;

		// Localisation attributes
		map<int, string> namesByLanguage;
		map<int, string> adjectivesByLanguage;
};


#endif // EU3COUNTRY_H_