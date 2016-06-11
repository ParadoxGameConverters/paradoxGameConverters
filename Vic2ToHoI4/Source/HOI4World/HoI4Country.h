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



#ifndef HoI4COUNTRY_H_
#define HoI4COUNTRY_H_


#include "HoI4Alignment.h"
#include "HoI4Army.h"
#include "HoI4Localisation.h"
#include "HoI4Province.h"
#include "HoI4Relations.h"
#include "../CountryMapping.h"
#include "../Mapper.h"
#include "../Color.h"
#include "Date.h"
#include "../V2World/V2Army.h"
#include <vector>
#include <set>
using namespace std;


class HoI4World;
class V2Country;
class HoI4Minister;
class HoI4Leader;



typedef struct
{
	wstring			name;
	wstring			localisationString;
	wstring			ideology;
	unsigned int	popularity;
	unsigned int	organization;
} HoI4Party;


class HoI4Country
{
	public:
		HoI4Country(string _tag, wstring _commonCountryFile, HoI4World* _theWorld, bool _newCountry = false);
		void	output() const;
		void	outputToCommonCountriesFile(FILE*) const;
		void	outputLocalisation(FILE*) const;
		void	outputAIScript() const;
		void	initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const wstring _vic2ideology, const CountryMapping& countryMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap, const V2Localisation& V2Localisations, governmentJobsMap governmentJobs, const namesMapping& namesMap, portraitMapping& portraitMap, const cultureMapping& cultureMap, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap);
		void	initFromHistory();
		void	consolidateProvinceItems(const inverseProvinceMapping& inverseProvinceMap, double& totalManpower, double& totalLeadership, double& totalIndustry);
		void	generateLeaders(leaderTraitsMap leaderTraits, const namesMapping& namesMap, portraitMapping& portraitMap);
		void	setAIFocuses(const AIFocusModifiers& focusModifiers);
		void	addMinimalItems(const inverseProvinceMapping& inverseProvinceMap);
		
		void	setTechnology(wstring tech, int level);
		void	addProvince(HoI4Province* _province);
		void	addArmy(HoI4RegGroup* army);
		void	lowerNeutrality(double amount);

		void	setFaction(wstring newFaction)	{ faction = newFaction; }
		void	setFactionLeader()				{ factionLeader = true; }

		HoI4Relations*								getRelations(wstring withWhom) const;
		HoI4Province*								getCapital();
		
		const map<wstring, HoI4Relations*>&	getRelations() const			{ return relations; }
		map<int, HoI4Province*>					getProvinces() const			{ return provinces; }
		string										getTag() const					{ return tag; }
		const V2Country*							getSourceCountry() const	{ return srcCountry; }
		wstring										getGovernment() const		{ return government; }
		bool											isNewCountry() const			{ return newCountry; }
		wstring										getFaction() const			{ return faction; }
		HoI4Alignment*								getAlignment()					{ return &alignment; }
		wstring										getIdeology() const			{ return ideology; }
		const set<string>&						getAllies() const				{ return allies; }
		set<string>&								editAllies()					{ return allies; }
		map<wstring, double>&					getPracticals()				{ return practicals; }
		const vector<HoI4RegGroup*>&			getArmies() const				{ return armies; }

	private:
		void			outputOOB()						const;
		void			outputPracticals(FILE*)		const;
		void			outputTech(FILE*)				const;
		void			outputParties(FILE*)			const;
		void			outputLeaders()				const;
		vector<int>	getPortProvinces(vector<int> locationCandidates, map<int, HoI4Province*> allProvinces);
		void			convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties, V2Party* rulingParty, wstring& rulingIdeology);

		HoI4World*							theWorld;
		const V2Country*					srcCountry;
		wstring								filename;
		bool									newCountry;	// true if this country is being added by the converter, i.e. doesn't already exist in HoI4

		string								tag;
		map<int, HoI4Province*>			provinces;
		int									capital;
		wstring								commonCountryFile;
		map<wstring, int>					technologies;
		wstring								government;
		HoI4Alignment						alignment;
		wstring								ideology;
		map<wstring, HoI4Relations*>	relations;
		vector<HoI4RegGroup*>			armies;
		Color									color;
		double								neutrality;
		double								nationalUnity;
		HoI4Localisation					localisation;
		wstring								faction;
		bool									factionLeader;
		set<string>							allies;
		map<wstring, double>				practicals;
		vector<HoI4Party>					parties;
		vector<HoI4Minister>				ministers;
		vector<HoI4Minister>				rulingMinisters;
		vector<HoI4Leader>				leaders;
		wstring								graphicalCulture;
		bool									majorNation;

		// AI focus modifiers
		double	seaModifier;
		double	tankModifier;
		double	airModifier;
		double	infModifier;

		// laws
		wstring				civil_law;
		wstring				conscription_law;
		wstring				economic_law;
		wstring				educational_investment_law;
		wstring				industrial_policy_laws;
		wstring				press_laws;
		wstring				training_laws;
};

#endif	// HoI4COUNTRY_H_
