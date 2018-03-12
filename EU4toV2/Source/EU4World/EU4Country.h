/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#ifndef EU4_COUNTRY_H_
#define EU4_COUNTRY_H_



#include "EU4Army.h"
#include "Color.h"
#include "Date.h"
#include "../CustomFlagMapper.h"
#include "CultureGroups.h"
#include "newParser.h"
#include <memory>
#include <optional>



namespace EU4
{
	class leader;
}
class EU4Loan;
class EU4Province;
class EU4Relations;
class EU4Version;



namespace EU4
{
	class Country: public commonItems::parser
	{
		public:
			Country(const std::string& countryTag, std::istream& theStream);

			// Add any additional information available from the specified country file.
			void readFromCommonCountry(const std::string& fileName, const std::string& fullFilename);

			void setLocalisationName(const string& language, const string& name);
			void setLocalisationAdjective(const string& language, const string& adjective);

			void						addProvince(EU4Province*);
			void						addCore(EU4Province*);
			void						setInHRE(bool _inHRE)								{ inHRE = _inHRE; }
			void						setEmperor(bool _emperor)							{ holyRomanEmperor = _emperor; }
			void						setCelestialEmperor(bool _celestialEmperor)			{ celestialEmperor = _celestialEmperor; }
			bool						hasModifier(string) const;
			int						hasNationalIdea(string) const;
			bool						hasFlag(string) const ;
			void						resolveRegimentTypes(const RegimentTypeMap& map);
			int						getManufactoryCount() const;
			int						numEmbracedInstitutions() const;
			void eatCountry(std::shared_ptr<Country> target, std::shared_ptr<Country> self);
			void						setColonialRegion(const string& region)		{ colonialRegion = region; }
			void						takeArmies(std::shared_ptr<Country>);
			void						clearArmies();
			const void				viveLaRevolution(bool revolting)					{ revolutionary = revolting; }

			bool cultureSurvivesInCores();

			string							getTag()										const { return tag; }
			vector<EU4Province*>			getProvinces()								const { return provinces; }
			vector<EU4Province*>			getCores()									const { return cores; }
			int								getCapital()								const { return capital; }
			bool							getInHRE()									const { return inHRE; }
			bool							getHolyRomanEmperor()					const { return holyRomanEmperor; }
			bool							getCelestialEmperor()					const { return celestialEmperor; }
			string							getTechGroup()								const { return techGroup; }
			vector<bool>					getEmbracedInstitutions()				const { return embracedInstitutions; }
			int								getIsolationism()						const { return isolationism; }
			string							getPrimaryCulture()						const { return primaryCulture; }
			vector<string>					getAcceptedCultures()					const { return acceptedCultures; }
			std::optional<EU4::cultureGroup> getCulturalUnion() const { return culturalUnion; }
			string							getReligion()								const { return religion; }
			double							getScore()									const { return score; }
			double							getStability()								const { return stability; }
			double							getAdmTech()								const { return admTech; }
			double							getDipTech()								const { return dipTech; }
			double							getMilTech()								const { return milTech; }
			double							getArmyInvestment()						const { return armyInvestment; }
			double							getNavyInvestment()						const { return navyInvestment; }
			double							getCommerceInvestment()					const { return commerceInvestment; }
			double							getIndustryInvestment()					const { return industryInvestment; }
			double							getCultureInvestment()					const { return cultureInvestment; }
			bool								getPossibleDaimyo()						const { return possibleDaimyo; }
			bool							getPossibleShogun()						const { return possibleShogun; }
			string							getGovernment()							const { return government; }
			map<string, EU4Relations*>	getRelations()								const { return relations; }
			vector<EU4Army*>				getArmies()									const { return armies; }
			bool								isCustom()									const { return customNation; }
			bool								isColony()									const { return colony; }
			string							getColonialRegion()						const { return colonialRegion; }
			double							getLibertyDesire()						const { return libertyDesire; }
			CustomFlag						getCustomFlag()							const { return customFlag; }
			bool								isRevolutionary()							const { return revolutionary; }
			tuple<int, int, int>			getRevolutionaryTricolour()			const { return revolutionaryTricolour; }
			string							getRandomName()							const { return randomName; }
			const map<string, int>& getNationalIdeas() const { return nationalIdeas; }
			std::vector<std::shared_ptr<EU4::leader>> getMilitaryLeaders() const { return militaryLeaders; }

			string	getName() const { return name; }
			string	getName(const string& language) const;
			string	getAdjective(const string& language) const;
			commonItems::Color getColor() const { return color; }

		private:
			void getGovernmentFromStream(const std::string& unused, std::istream& theStream);
			void determineJapaneseRelations();
			void determineInvestments();
			void determineLibertyDesire();
			void							clearProvinces();
			void							clearCores();

			string							tag;						// the tag for the EU4 nation
			vector<EU4Province*>			provinces;				// the EU4 provinces this nations holds
			vector<EU4Province*>			cores;					// the EU4 provinces this nation has cores on
			bool							inHRE;					// if this country is an HRE member
			bool							holyRomanEmperor;		// if this country is the emperor of the HRE
			bool							celestialEmperor;		// if this country is the celestial emperor
			int								capital;					// the EU4 province that is this nation's capital
			string							techGroup;				// the tech group for this nation
			vector<bool>					embracedInstitutions; // the institutions this nation has embraced
			int								isolationism;			// the isolationism of the country (for Shinto nations with Mandate of Heaven)
			string							primaryCulture;		// the primary EU4 culture of this nation
			vector<string>					acceptedCultures;		// the accepted EU4 cultures for this nation
			std::optional<EU4::cultureGroup> culturalUnion;
			string							religion;				// the accepted religion of this country
			double							score;					// this country's EU4 score
			double							stability;				// the stability of this nation
			double							admTech;					// the admin tech of this nation
			double							dipTech;					// the diplo tech of this nation
			double							milTech;					// the mil tech of this nation
			double							armyInvestment;		// the amount of 'investment' into army tech (determined by ideas)
			double							navyInvestment;		// the amount of 'investment' into navy tech (determined by ideas)
			double							commerceInvestment;	// the amount of 'investment' into commerce tech (determined by ideas)
			double							industryInvestment;	// the amount of 'investment' into industry tech (determined by ideas)
			double							cultureInvestment;	// the amount of 'investment' into culture tech (determined by ideas)
			map<string, bool>				flags;					// any flags set for this country
			map<string, bool>				modifiers;				// any modifiers set for this country
			bool								possibleDaimyo;		// if this country is possibly a daimyo
			bool							possibleShogun;			// if this country is the shogun
			std::vector<std::shared_ptr<EU4::leader>> militaryLeaders;
			string							government;				// the government type
			map<string, EU4Relations*>	relations;				// the relations with other nations
			vector<EU4Army*>				armies;					// this nation's armies and navies
			map<string, int>				nationalIdeas;			// the national ideas for this country
			double							legitimacy;				// the legitimacy of this nation
			bool								customNation;			// whether or not this is a custom or random nation
			bool								colony;					// whether or not this country is a colony
			std::string overlord;
			string							colonialRegion;		// the colonial region, if this country is a colony
			double							libertyDesire;			// the amount of liberty desire
			string							randomName;				// the new name of this nation in Random World
			CustomFlag						customFlag;				// the custom flag
			bool								revolutionary;			// does this country wave the glorious tricoloured banner of the revolution
			tuple<int, int, int>			revolutionaryTricolour; // the glorious tricoloured banner of the revolution
		

			// Localisation attributes
			string	name;			// the name of this country
			string	adjective;	// the adjective for this country
			commonItems::Color color;

			map<string, string> namesByLanguage;		// the names of this country in different localisations
			map<string, string> adjectivesByLanguage;	// the adjectives for this country in different localisations
	};
}



#endif // EU4_COUNTRY_H_