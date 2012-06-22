#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include "EU3Army.h"
#include "Date.h"

class EU3Province;
class EU3Relations;
class EU3Loan;
class EU3Leader;



class EU3Country
{
	public:
		EU3Country(Object* obj);

		void							addProvince(EU3Province*);
		void							addCore(EU3Province*);
		bool							hasModifier(string);
		bool							hasNationalIdea(string);
		bool							hasFlag(string);
		void							resolveRegimentTypes(const RegimentTypeMap& map);
		int							getManufactoryCount();
		double						inflationAdjust(double input);
		double						getBadboyLimit();
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
		double						getLandInvestment()						const { return landInvestment; };
		double						getNavalInvestment()						const { return navalInvestment; };
		double						getTradeInvestment()						const { return tradeInvestment; };
		double						getProductionInvestment()				const { return productionInvestment; };
		double						getGovernmentInvestment()				const { return governmentInvestment; };
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

		double						getTreasury()								{ return inflationAdjust(treasury); };

	private:
		void							checkIdea(Object*, string);
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
		double						landInvestment;
		double						navalInvestment;
		double						tradeInvestment;
		double						productionInvestment;
		double						governmentInvestment;
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
		map<string, bool>			nationalIdeas;
		double						treasury;
		date							last_bankrupt;
		vector<EU3Loan*>			loans;
		double						diplomats;
		double						badboy;

		double						legitimacy;
		double						inflation;
};


#endif // EU3COUNTRY_H_