#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "..\Date.h"
#include "..\Mappers.h"
#include "..\CK2World\CK2Title.h"
using namespace std;


class CK2Title;
class EU3Ruler;
class EU3Advisor;
class EU3History;
class EU3Province;
class EU3World;
class EU3Tech;
class EU3Diplomacy;
struct EU3Agreement;
class CK2Province;
class EU3Army;

class EU3Country
{
	public:
		EU3Country(EU3World* world, string tag, string countryFile, date startDate, const EU3Tech* techData);
		EU3Country(CK2Title*, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap);

		void						output(FILE*);
		void						determineLearningScore();
		void						addAcceptedCultures();
		void						determineGovernment(double prestigeFactor);
		void						determineEconomy(const cultureGroupMapping& cultureGroups, const map<string, double>& unitPrices);
		double					getTradeEffeciency();
		double					getProductionEffeciency();
		void						setPreferredUnitType();
		void						determineTechLevels(const vector<double>& avgTechLevels, const EU3Tech* techData);
		void						determineTechInvestment(const EU3Tech* techData, date startDate);
		void						determineStartingAgents();
		vector<EU3Country*>	convertVassals(int initialScore, EU3Diplomacy* diplomacy);
		vector<EU3Country*>	eatVassals();
		void						eatVassal(EU3Country*);
		void						replaceWith(EU3Country* convertedCountry, const provinceMapping& provinceMappings);
		void						convertArmies(inverseProvinceMapping inverseProvinceMap, map<int, EU3Province*> provinces);

		void		addLiege(EU3Country* _liege)			{ liege = _liege; if (liege != NULL) _liege->addVassal(this); };
		void		addVassal(EU3Country* _vassal)		{ vassals.push_back(_vassal); };
		void		addProvince(EU3Province* province)	{ provinces.push_back(province); };
		void		addCore(EU3Province* core)				{ cores.push_back(core); };
		void		setAbsorbScore(int _score)				{ absorbScore = _score; };
		void		setTechGroup(string _techGroup)		{ techGroup = _techGroup; };
		void		setTag(string _tag)						{ tag = _tag; };
		void		addAdvisor(EU3Advisor* _advisor)		{ advisors.push_back(_advisor); };
		void		setRelations(EU3Country* other, int value)	{ relations.insert(make_pair(other, value)); };
		void		setElector(bool _elector)				{ elector = _elector; };

		CK2Title*				getSrcCountry()			const { return src; };
		EU3Country*				getLiege()					const { return liege; };
		vector<EU3Country*>	getVassals()				const { return vassals; };
		vector<EU3Province*>	getProvinces()				const { return provinces; };
		vector<EU3Province*>	getCores()					const { return cores; };
		vector<EU3Advisor*>	getAdvisors()				const { return advisors; };
		double					getLearningScore()		const { return learningScore; };
		int						getAbsorbScore()			const	{ return absorbScore; };
		string					getTag()						const { return tag; };
		string					getGovernment()			const { return government; };
		string					getPrimaryCulture()		const { return primaryCulture; };
		vector<string>			getAcceptedCultures()	const { return acceptedCultures; };
		string					getReligion()				const { return religion; };
		string					getTechGroup()				const { return techGroup; };
		int						getCapital()				const { return capital; };
		int						getStability()				const { return stability; };
		bool						hasProvinces()				const { return !provinces.empty(); };
		bool						hasCores()					const { return !cores.empty(); };
		bool						hasVassals()				const { return !vassals.empty(); };

	private:
		CK2Title*				src;
		EU3Country*				liege;
		vector<EU3Country*>	vassals;
		vector<EU3Province*>	provinces;
		vector<EU3Province*> cores;
		vector<EU3Advisor*>	advisors;
		double					learningScore;
		int						absorbScore;

		string						tag;
		string						historyFile;
		string						government;
		string						primaryCulture;
		vector<string>				acceptedCultures;
		string						religion;
		EU3Ruler*					monarch;
		EU3Ruler*					heir;
		EU3Ruler*					regent;
		vector<EU3History*>		history;
		vector<EU3Ruler*>			previousMonarchs;
		string						techGroup;
		double						governmentTech;
		double						productionTech;
		double						tradeTech;
		double						navalTech;
		double						landTech;
		double						governmentTechInvestment;
		double						productionTechInvestment;
		double						tradeTechInvestment;
		double						navalTechInvestment;
		double						landTechInvestment;
		vector<EU3Agreement*>	agreements;
		map<EU3Country*, int>	relations;

		int						capital;
		int						stability;
		double					stabilityInvestment;
		double					prestige;

		double					estimatedIncome;
		double					estimatedTax;
		double					estimatedGold;
		double					estimatedProduction;
		double					estimatedTolls;

		bool						daimyo;
		bool						japaneseEmperor;
		bool						elector;

		vector<EU3Army*>		armies;
		double					manpower;
		string					infantry;
		string					cavalry;
		string					bigShip;
		string					galley;
		string					transport;

		double					merchants;
		double					colonists;
		double					diplomats;
		double					missionaries;
		double					spies;
		double					magistrates;
};



#endif	// EU3COUNTRY_H_