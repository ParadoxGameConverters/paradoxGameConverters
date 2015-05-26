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



#ifndef HOI3COUNTRY_H_
#define HOI3COUNTRY_H_


#include "HoI3Alignment.h"
#include "HoI3Army.h"
#include "HoI3Localisation.h"
#include "HoI3Province.h"
#include "../CountryMapping.h"
#include "../Mapper.h"
#include "../Color.h"
#include "../Date.h"
#include "../V2World/V2Army.h"
#include <vector>
#include <set>
using namespace std;


class HoI3World;
class V2Country;
class HoI3Relations;
class HoI3Minister;



typedef struct
{
	string			name;
	string			localisationString;
	string			ideology;
	unsigned int	popularity;
	unsigned int	organization;
} HoI3Party;


class HoI3Country
{
	public:
		HoI3Country(string _tag, string _commonCountryFile, HoI3World* _theWorld, bool _newCountry = false);
		void								output() const;
		void								outputToCommonCountriesFile(FILE*) const;
		void								outputOOB() const;
		void								initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _vic2ideology, vector<string> outputOrder, const CountryMapping& countryMap, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitsMap, const cultureMapping& cultureMap);
		void								initFromHistory();
		void								convertArmies(const map<int,int>& leaderIDMap, const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces, vector<int> port_whitelist);
		void								addRelation(HoI3Relations* newRelation);
		void								addProvince(HoI3Province* _province);

		HoI3Relations*					getRelations(string withWhom) const;
		void								getNationalValueScores(int& liberty, int& equality, int& order, const map<string, int>& orderIdeas, const map<string, int>& libertyIdeas, const map<string, int>& equalityIdeas);
		
		void								isANewCountry(void)							{ newCountry = true; };

		vector<HoI3Province*>		getProvinces() const { return provinces; };
		string							getTag() const { return tag; };
		const V2Country*				getSourceCountry() const { return srcCountry; };
		string							getGovernment() const { return government; };
		int								getCapital() const { return capital; };
		bool								isNewCountry() const { return newCountry; };

		void								outputLocalisation(FILE*) const;
		
		string removeAccented(string str);
		const std::map<string, HoI3Relations*> &getRelations() const;
		string getFaction() const { return faction; };
		void setFaction(string newFaction) { faction = newFaction; };
		HoI3Alignment* getAlignment() { return &alignment; };
		string getIdeology() const { return ideology; };
		const set<string> &getAllies() const { return allies; };
		set<string> &editAllies() { return allies; };
		void setTechnology(string tech, int level);

		void addArmy(HoI3RegGroup army);
		map<string, double>& getPracticals() { return practicals; };
		const vector<HoI3RegGroup>& getArmies() const { return armies; };

	private:
		void			outputPracticals(FILE*)	const;
		void			outputTech(FILE*)			const;
		void			outputElection(FILE*)	const;
		void			outputParties(FILE*)		const;
		vector<int>	getPortProvinces(vector<int> locationCandidates, map<int, HoI3Province*> allProvinces);
		void			convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, string& rulingIdeology);

		HoI3World*							theWorld;
		const V2Country*					srcCountry;
		string								filename;
		bool									newCountry;	// true if this country is being added by the converter, i.e. doesn't already exist in HoI3

		string								tag;
		vector<HoI3Province*>			provinces;
		int									capital;
		string								commonCountryFile;
		map<string, int>					technologies;
		string								government;
		HoI3Alignment						alignment;
		string								ideology;
		map<string, HoI3Relations*>	relations;
		vector<HoI3RegGroup>				armies;
		double								money;
		double								diploPoints;
		double								badboy;
		Color									color;
		double								neutrality;
		double								nationalUnity;
		HoI3Localisation					localisation;
		string								faction;
		set<string>							allies;
		map<string, double>				practicals;
		vector<HoI3Party>					parties;
		vector<HoI3Minister>				ministers;
		vector<HoI3Minister>				rulingMinisters;
		string								graphicalCulture;

		// laws
		string				civil_law;
		string				conscription_law;
		string				economic_law;
		string				educational_investment_law;
		string				industrial_policy_laws;
		string				press_laws;
		string				training_laws;
};

#endif	// HoI3COUNTRY_H_