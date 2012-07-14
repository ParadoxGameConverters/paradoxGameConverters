#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_



#include "Mapper.h"
#include "EU3Army.h"
#include "V2Inventions.h"
#include "V2TechSchools.h"
#include "Date.h"
#include <vector>
using namespace std;

class EU3World;
class EU3Country;
class V2World;
class V2State;
class V2Province;
class V2Relations;
class V2Army;
class V2Reforms;
class V2UncivReforms;
class V2Factory;
class V2Creditor;
class V2Leader;
class V2LeaderTraits;
struct V2Party;



class V2Country
{
	public:
		V2Country(string _tag, string _countryFile, map<int, V2Party*> _parties, V2World* _theWorld);
		void								output(FILE*);
		void								initFromEU3Country(const EU3Country* _srcCountry, vector<string> outputOrder, countryMapping countryMap, cultureMapping cultureMap, religionMapping religionMap, unionCulturesMap unionCultures, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<V2TechSchool> techSchools, map<int,int>& leaderMap, const V2LeaderTraits& lt);
		void								initFromHistory();
		void								addState(V2State* newState);
		void								convertArmies(const map<int,int>& leaderIDMap, double cost_per_regiment[num_reg_categories], const inverseProvinceMapping& inverseProvinceMap, vector<V2Province*> allProvinces, vector<int> port_whitelist);
		void								setNationalIdea(int& libertyLeft, int& equalityLeft);
		bool								addFactory(V2Factory* factory);
		void								addRailroadtoCapitalState();
		void								setupPops(EU3World& sourceWorld);
		void								setArmyTech(double mean, double scale, double stdDev);
		void								setNavyTech(double mean, double scale, double stdDev);
		void								setCommerceTech(double mean, double scale, double stdDev);
		void								setIndustryTech(double mean, double scale, double stdDev);
		void								setCultureTech(double mean, double scale, double stdDev);

		V2Relations*					getRelations(string withWhom) const;
		
		void								addPrestige(double additionalPrestige) { prestige += additionalPrestige; };
		void								addResearchPoints(double newPoints) { researchPoints += newPoints; };
		void								addTech(string newTech) { techs.push_back(newTech); };

		vector<V2Province*>			getProvinces() const { return provinces; };
		string							getTag() const { return tag; };
		bool								isCivilized() const { return civilized; };
		string							getPrimaryCulture() const { return primaryCulture; };
		vector<string>					getAcceptedCultures() const { return acceptedCultures; };
		const EU3Country*				getSourceCountry() const { return srcCountry; };
		inventionStatus				getInventionState(inventionType invention) const { return inventions[invention]; };
		double							getReactionary() const { return upperHouseReactionary; };
		double							getConservative() const { return upperHouseConservative; };
		double							getLiberal() const { return upperHouseLiberal; };
		vector< pair<int, int> >	getReactionaryIssues() const { return reactionaryIssues; };
		vector< pair<int, int> >	getConservativeIssues() const { return conservativeIssues; };
		vector< pair<int, int> >	getLiberalIssues() const { return liberalIssues; };
		double							getLiteracy() const { return literacy; };
		int								getCapital() const { return capital; };
	private:
		void			outputTech(FILE*);
		void			outputInventions(FILE*);
		void			outputElection(FILE*);
		void			outputParties(FILE*);
		void			setIssues();
		void			setRulingParty();
		void			sortRelations(const vector<string>& order);
		void			addLoan(string creditor, double size, double interest);
		int			addRegimentToArmy(V2Army* army, RegimentCategory rc, const inverseProvinceMapping& inverseProvinceMap, vector<V2Province*> allProvinces);
		vector<int>	getPortProvinces(vector<int> locationCandidates, vector<V2Province*> allProvinces);
		V2Army*		getArmyForRemainder(RegimentCategory rc);
		V2Province*	getProvinceForExpeditionaryArmy();

		V2World*							theWorld;
		const EU3Country*				srcCountry;
		string							tag;
		vector<V2State*>				states;
		vector<V2Province*>			provinces;
		int								capital;
		bool								civilized;
		string							primaryCulture;
		vector<string>					acceptedCultures;
		string							religion;
		map<int, V2Party*>			parties;
		int								rulingParty;
		string							countryFile;
		double							prestige;
		double							leadership;
		double							plurality;
		vector<string>					techs;
		inventionStatus				inventions[naval_exercises];
		V2UncivReforms*				uncivReforms;
		double							researchPoints;
		string							techSchool;
		string							government;
		double							upperHouseReactionary;
		double							upperHouseConservative;
		double							upperHouseLiberal;
		vector< pair<int, int> >	reactionaryIssues;
		vector< pair<int, int> >	conservativeIssues;
		vector< pair<int, int> >	liberalIssues;
		vector<V2Relations*>			relations;
		vector<V2Army*>				armies;
		V2Reforms*						reforms;
		string							nationalValue;
		double							money;
		date								lastBankrupt;
		map<string, V2Creditor*>	creditors;
		double							bankReserves;
		double							diploPoints;
		double							badboy;
		vector<V2Leader*>				leaders;
		double							literacy;
};

bool ProvinceRegimentCapacityPredicate(V2Province* prov1, V2Province* prov2);


#endif	// V2COUNTRY_H_