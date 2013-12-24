#ifndef EU4COUNTRY_H_
#define EU4COUNTRY_H_



#include "EU4Army.h"
#include "Date.h"

class EU4Province;
class EU4Relations;
class EU4Loan;
class EU4Leader;


class EU4Country
{
	public:
		EU4Country(Object* obj);

		void						addProvince(EU4Province*);
		void						addCore(EU4Province*);
		bool						hasModifier(string) const;
		bool						hasNationalIdea(string) const;
		bool						hasFlag(string) const ;
		void						resolveRegimentTypes(const RegimentTypeMap& map);
		int							getManufactoryCount() const;
		double						inflationAdjust(double input) const;
		double						getBadboyLimit() const;
		void						eatCountry(EU4Country* target);

		string						getTag()										const { return tag; };
		vector<EU4Province*>		getProvinces()								const { return provinces; };
		vector<EU4Province*>		getCores()									const { return cores; };
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
		double						getAdmTech()								const { return admTech; };
		double						getDipTech()								const { return dipTech; };
		double						getMilTech()								const { return milTech; };
/*		double						getLandTech()								const { return landTech; };
		double						getNavalTech()								const { return navalTech; };
		double						getTradeTech()								const { return tradeTech; };
		double						getProductionTech()						const { return productionTech; };
		double						getGovernmentTech()						const { return governmentTech; };
*/		double						getEstimatedMonthlyIncome()			const { return estMonthlyIncome; };
		double						getLandInvestment()						const { return landInvestment; };
		double						getNavalInvestment()						const { return navalInvestment; };
		double						getTradeInvestment()						const { return tradeInvestment; };
		double						getProductionInvestment()				const { return productionInvestment; };
		double						getGovernmentInvestment()				const { return governmentInvestment; };
		bool						getPossibleDaimyo()						const { return possibleDaimyo; };
		string						getGovernment()							const { return government; };
		vector<EU4Relations*>		getRelations()								const { return relations; };
		vector<EU4Army*>			getArmies()									const { return armies; };
		int							getCentralizationDecentralization()	const { return centralization_decentralization; };
		int							getAristocracyPlutocracy()				const { return aristocracy_plutocracy; };
		int							getSerfdomFreesubjects()				const { return serfdom_freesubjects; };
		int							getInnovativeNarrowminded()			const { return innovative_narrowminded; };
		int							getMercantilismFreetrade()				const { return mercantilism_freetrade; };
		int							getOffensiveDefensive()					const { return offensive_defensive; };
		int							getLandNaval()								const { return land_naval; };
		int							getQualityQuantity()						const { return quality_quantity; };
		date						getLastBankrupt()							const { return last_bankrupt; };
		vector<EU4Loan*>			getLoans()									const { return loans; };
		double						getDiplomats()								const { return diplomats; };
		double						getBadboy()									const { return badboy; };
		vector<EU4Leader*>			getLeaders()								const { return leaders; };

		double						getTreasury()								const	{ return inflationAdjust(treasury); };

	private:
		void							checkIdea(const Object*, const string);
		void							clearProvinces();
		void							clearCores();

		string						tag;
		vector<EU4Province*>		provinces;
		vector<EU4Province*>		cores;
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
		double						admTech;
		double						dipTech;
		double						milTech;
		double						estMonthlyIncome;
		double						landInvestment;
		double						navalInvestment;
		double						tradeInvestment;
		double						productionInvestment;
		double						governmentInvestment;
		map<string, bool>			flags;
		map<string, bool>			modifiers;
		bool						possibleDaimyo;
		vector<EU4Leader*>			leaders;
		string						government;
		vector<EU4Relations*>		relations;
		vector<EU4Army*>			armies; // and navies...
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
		date						last_bankrupt;
		vector<EU4Loan*>			loans;
		double						diplomats;
		double						badboy;

		double						legitimacy;
		double						inflation;
};


#endif // EU4COUNTRY_H_