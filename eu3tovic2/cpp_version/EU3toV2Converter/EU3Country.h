#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_


#include "EU3Province.h"
#include "EU3Relations.h"
#include "EU3Army.h"
#include "EU3Loan.h"
#include "EU3Leader.h"


class EU3Country
{
	public:
		void						init(Object* obj);
		string					getTag();
		void						addProvince(EU3Province*);
		void						clearProvinces();
		void						addCore(EU3Province*);
		void						clearCores();
		vector<EU3Province*>	getProvinces();
		vector<EU3Province*>	getCores();
		int						getCapital();
		int						getNationalFocus();
		string					getTechGroup();
		string					getPrimaryCulture();
		vector<string>			getAcceptedCultures();
		string					getReligion();
		double					getPrestige();
		double					getCulture();
		double					getArmyTradition();
		double					getNavyTradition();
		double					getStability();
		double					getLandTech();
		double					getNavalTech();
		double					getTradeTech();
		double					getProductionTech();
		double					getGovernmentTech();
		double					getLandInvestment();
		double					getNavalInvestment();
		double					getTradeInvestment();
		double					getProductionInvestment();
		double					getGovernmentInvestment();
		vector<string>			getFlags();
		bool						hasModifier(string);
		bool						getPossibleDaimyo();
		string					getGovernment();
		vector<EU3Relations>	getRelations();
		vector<EU3Army>		getArmies();
		void						resolveRegimentTypes(const RegimentTypeMap& map);
		int						getCentralizationDecentralization();
		int						getAristocracyPlutocracy();
		int						getSerfdomFreesubjects();
		int						getInnovativeNarrowminded();
		int						getMercantilismFreetrade();
		int						getOffensiveDefensive();
		int						getLandNaval();
		int						getQualityQuantity();
		vector<string>			getNationalIdeas();
		bool						hasNationalIdea(string);
		int						getManufactoryCount();
		double					getTreasury();
		double					inflationAdjust(double input);
		date						getLastBankrupt();
		vector<EU3Loan>		getLoans();
		double					getEstimatedMonthlyIncome();
		double					getDiplomats();
		double					getBadboy();
		double					getBadboyLimit();
		vector<EU3Leader>		getLeaders();
		void						eatCountry(EU3Country* target);

	private:
		string					tag;
		vector<EU3Province*>	provinces;
		vector<EU3Province*>	cores;
		int						capital;
		int						nationalFocus;
		string					techGroup;
		string					primaryCulture;
		vector<string>			acceptedCultures;
		string					religion;
		double					prestige;
		double					culture;
		double					armyTradition;
		double					navyTradition;
		double					stability;
		double					landTech;
		double					navalTech;
		double					tradeTech;
		double					productionTech;
		double					governmentTech;
		double					landInvestment;
		double					navalInvestment;
		double					tradeInvestment;
		double					productionInvestment;
		double					governmentInvestment;
		vector<string>			flags;
		vector<string>			modifiers;
		bool						possibleDaimyo;
		string					government;
		vector<EU3Relations>	relations;
		vector<EU3Army>		armies; // and navies...
		int						centralization_decentralization;
		int						aristocracy_plutocracy;
		int						serfdom_freesubjects;
		int						innovative_narrowminded;
		int						mercantilism_freetrade;
		int						offensive_defensive;
		int						land_naval;
		int						quality_quantity;
		vector<string>			nationalIdeas;
		double					treasury;
		date						last_bankrupt;
		vector<EU3Loan>		loans;
		double					estMonthlyIncome;
		double					diplomats;
		double					badboy;
		vector<EU3Leader>		leaders;

		double					legitimacy;
		double					inflation;
};


#endif // EU3COUNTRY_H_