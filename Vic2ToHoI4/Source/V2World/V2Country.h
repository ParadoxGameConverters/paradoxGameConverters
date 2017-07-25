/*Copyright (c) 2017 The Paradox Game Converters Project

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



#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_



#include "../Mappers/Mapper.h"
#include "../Color.h"
#include "V2Inventions.h"
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <string>
using namespace std;



class		V2Army;
class		V2Leader;
struct	V2Party;
class		V2Province;
class		V2Relations;
class		Vic2State;



class V2Country
{
public:
	V2Country(shared_ptr<Object> obj);

	void addProvince(const pair<const int, V2Province*>& province) { provinces.insert(province); }
	void setColor(const Color& newColor) { color = newColor; }
	void setAsGreatNation() { greatNation = true; }
	void addCore(V2Province* core) { cores.push_back(core); }
	void replaceCores(vector<V2Province*> newCores) { cores.swap(newCores); }

	void eatCountry(V2Country* target);
	void putProvincesInStates();
	void determineEmployedWorkers();
	void setLocalisationNames();
	void setLocalisationAdjectives();

	map<string, V2Relations*> getRelations() const { return relations; }
	vector<Vic2State*> getStates() const { return states; }
	string getTag() const { return tag; }
	string getPrimaryCulture() const { return primaryCulture; }
	string getPrimaryCultureGroup() const { return primaryCultureGroup; }
	set<string> getAcceptedCultures() const { return acceptedCultures; }
	bool isAnAcceptedCulture(const string& culture) const { return (acceptedCultures.count(culture) > 0); }
	set<string> getInventions() const { return inventions; }
	string getGovernment() const { return government; }
	int getCapital() const { return capital; }
	set<string> getTechs() const { return techs; }
	Color getColor() const { return color; }
	double getEducationSpending() const { return educationSpending; }
	double getMilitarySpending() const { return militarySpending; }
	vector<V2Army*> getArmies() const { return armies; }
	vector<V2Leader*> getLeaders() const { return leaders; }
	double getRevanchism() const { return revanchism; }
	double getWarExhaustion() const { return warExhaustion; }
	double getBadBoy() const { return badboy; }
	map<string, string> getAllReforms() const { return reformsArray; }
	bool isGreatNation() const { return greatNation; }
	map<int, V2Province*> getProvinces() const { return provinces; }
	vector<V2Province*> getCores() const { return cores; }
	bool isEmpty() const { return ((cores.size() == 0) && (provinces.size() == 0)); }
	bool isCivilized() const { return civilized; }
	bool isHuman() const { return human; }
	map<string, double> getUpperHouseComposition() const { return upperHouseComposition; }

	string getReform(const string& reform) const;
	string getName(const string& language) const;
	string getAdjective(const string& language) const;
	double getUpperHousePercentage(const string& ideology) const;
	long getEmployedWorkers() const;
	V2Party* getRulingParty(const vector<V2Party*>& allParties) const;
	set<V2Party*> getActiveParties(const vector<V2Party*>& allParties) const;
	bool hasCoreOnCapital() const;

private:
	void readInDomainNameAndAdjective(const shared_ptr<Object> countryObj);
	void readInCapital(const shared_ptr<Object> countryObj);
	void readInCultures(const shared_ptr<Object> countryObj);
	void readInCivilized(const shared_ptr<Object> countryObj);
	void readInTechnology(const shared_ptr<Object> countryObj);
	void readInInventions(const shared_ptr<Object> countryObj);
	void readInPoliticalParties(const shared_ptr<Object> countryObj);
	void readInSpending(const shared_ptr<Object> countryObj);
	void readInRevanchism(const shared_ptr<Object> countryObj);
	void readInWarExhaustion(const shared_ptr<Object> countryObj);
	void readInBadBoy(const shared_ptr<Object> countryObj);
	void readInReforms(shared_ptr<Object> countryObj);
	void readInGovernment(const shared_ptr<Object> countryObj);
	void readInUpperHouse(const shared_ptr<Object> countryObj);
	void readInRelations(shared_ptr<Object> countryObj);
	bool isCountryTag(string potentialTag);
	void readInMilitary(const shared_ptr<Object> countryObj);
	void readInLeaders(const shared_ptr<Object> countryObj);
	void readInStates(const shared_ptr<Object> countryObj);
	void createNewState(const shared_ptr<Object> stateObj);
	void detectIfHuman(const shared_ptr<Object> stateObj);

	void setLocalisationName(const string& language, const string& name);
	void setLocalisationAdjective(const string& language, const string& adjective);

	string tag;
	Color color;

	vector<Vic2State*> states;
	map<int, V2Province*> provinces;
	vector<V2Province*> cores;
	int capital;

	string primaryCulture;
	string primaryCultureGroup;
	set<string> acceptedCultures;

	set<string> techs;
	set<string> inventions;

	map<string, V2Relations*> relations;
	bool greatNation;
	bool civilized;

	vector<V2Army*> armies;
	vector<V2Leader*> leaders;

	double educationSpending;
	double militarySpending;

	double revanchism;
	double warExhaustion;
	double badboy;

	string government;
	map<string, string> reformsArray;
	map<string, double> upperHouseComposition;
	unsigned	int rulingPartyID;
	vector<unsigned int> activePartyIDs;

	string domainName;
	string domainAdjective;
	map<string, string> namesByLanguage;
	map<string, string> adjectivesByLanguage;

	bool human;
};

#endif	// V2COUNTRY_H_