/*Copyright (c) 2018 The Paradox Game Converters Project

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



#ifndef VIC2_COUNTRY_H_
#define VIC2_COUNTRY_H_



#include "../Color.h"
#include "Date.h"
#include "Party.h"
#include "newParser.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>



namespace Vic2
{

class Army;
class Leader;
class Province;
class Relations;
class State;


class Country: commonItems::parser
{
	public:
		explicit Country(const std::string& theTag, std::istream& theStream);

		void addProvince(const std::pair<const int, Province*>& province) { provinces.insert(province); }
		void setColor(const ConverterColor::Color& newColor) { color = newColor; }
		void setAsGreatNation() { greatNation = true; }
		void addCore(Province* core) { cores.push_back(core); }
		void replaceCores(std::vector<Province*> newCores) { cores.swap(newCores); }
		void setShipNames(std::map<std::string, std::vector<std::string>> newShipNames) { shipNames = newShipNames; }

		void eatCountry(Country* target);
		void putProvincesInStates();
		void determineEmployedWorkers();
		void setLocalisationNames();
		void setLocalisationAdjectives();
		void handleMissingCulture();

		std::map<std::string, const Relations*> getRelations() const { return relations; }
		std::vector<State*> getStates() const { return states; }
		std::string getTag() const { return tag; }
		std::string getPrimaryCulture() const { return primaryCulture; }
		std::string getPrimaryCultureGroup() const { return primaryCultureGroup; }
		std::set<std::string> getAcceptedCultures() const { return acceptedCultures; }
		bool isAnAcceptedCulture(const std::string& culture) const { return (acceptedCultures.count(culture) > 0); }
		std::set<std::string> getInventions() const { return inventions; }
		std::string getGovernment() const { return government; }
		date getLastElection() const { return lastElection; }
		int getCapital() const { return capital; }
		std::set<std::string> getTechs() const { return techs; }
		const ConverterColor::Color& getColor() const { return color; }
		std::vector<const Army*> getArmies() const { return armies; }
		std::vector<const Leader*> getLeaders() const { return leaders; }
		double getRevanchism() const { return revanchism; }
		double getWarExhaustion() const { return warExhaustion; }
		double getBadBoy() const { return badboy; }
		std::map<std::string, std::string> getAllReforms() const { return reformsArray; }
		bool isGreatNation() const { return greatNation; }
		std::map<int, Province*> getProvinces() const { return provinces; }
		std::vector<Province*> getCores() const { return cores; }
		bool isEmpty() const { return ((cores.size() == 0) && (provinces.size() == 0)); }
		bool isCivilized() const { return civilized; }
		bool isHuman() const { return human; }
		std::map<std::string, double> getUpperHouseComposition() const { return upperHouseComposition; }

		std::optional<std::string> getReform(const std::string& reform) const;
		std::optional<std::string> getName(const std::string& language) const;
		std::optional<std::string> getAdjective(const std::string& language) const;
		double getUpperHousePercentage(const std::string& ideology) const;
		long getEmployedWorkers() const;
		std::optional<const Vic2::Party> getRulingParty(const std::vector<Vic2::Party>& allParties) const;
		std::set<Vic2::Party, std::function<bool (const Vic2::Party&, const Vic2::Party&)>> getActiveParties(const std::vector<Vic2::Party>& allParties) const;
		bool hasCoreOnCapital() const;
		std::vector<std::string> getShipNames(std::string category) const;
		double getAverageMilitancy() const;
		float getAverageIssueSupport(const std::string& issueName) const;

	private:
		Country(const Country&) = delete;
		Country& operator=(const Country&) = delete;

		void setLocalisationName(const std::string& language, const std::string& name);
		void setLocalisationAdjective(const std::string& language, const std::string& adjective);

		std::map<std::string, int> determineCultureSizes();
		std::string selectLargestCulture(const std::map<std::string, int>& cultureSizes);

		std::string tag = "";
		ConverterColor::Color color;

		std::vector<State*> states;
		std::map<int, Province*> provinces;
		std::vector<Province*> cores;
		int capital = 0;

		std::string primaryCulture = "";
		std::string primaryCultureGroup = "";
		std::set<std::string> acceptedCultures;

		std::set<std::string> techs;
		std::set<std::string> inventions;

		std::map<std::string, const Relations*> relations;
		bool greatNation = false;
		bool civilized = false;

		std::vector<const Army*> armies;
		std::vector<const Leader*> leaders;

		double revanchism = 0.0;
		double warExhaustion = 0.0;
		double badboy = 0.0;

		std::string government = "";
		std::map<std::string, std::string> reformsArray;
		std::map<std::string, double> upperHouseComposition;
		unsigned	int rulingPartyID = 0; 	// Bad value, but normal for Rebel faction.
		std::vector<unsigned int> activePartyIDs;
		date lastElection;

		std::string domainName = "";
		std::string domainAdjective = "";
		std::map<std::string, std::string> namesByLanguage;
		std::map<std::string, std::string> adjectivesByLanguage;
		std::map<std::string, std::vector<std::string> > shipNames;

		bool human = false;
};

}

#endif	// VIC2_COUNTRY_H_