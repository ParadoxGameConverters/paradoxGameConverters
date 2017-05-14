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
	V2Country(Object* obj);

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
	void readInDomainNameAndAdjective(const Object* countryObj);
	void readInCapital(const Object* countryObj);
	void readInCultures(const Object* countryObj);
	void readInCivilized(const Object* countryObj);
	void readInTechnology(const Object* countryObj);
	void readInInventions(const Object* countryObj);
	void readInPoliticalParties(const Object* countryObj);
	void readInSpending(const Object* countryObj);
	void readInRevanchism(const Object* countryObj);
	void readInWarExhaustion(const Object* countryObj);
	void readInBadBoy(const Object* countryObj);
	void readInReforms(Object* countryObj);
	void readInGovernment(const Object* countryObj);
	void readInUpperHouse(const Object* countryObj);
	void readInRelations(Object* countryObj);
	bool isCountryTag(string potentialTag);
	void readInMilitary(const Object* countryObj);
	void readInLeaders(const Object* countryObj);
	void readInStates(const Object* countryObj);
	void createNewState(const Object* stateObj);
	void detectIfHuman(const Object* stateObj);

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

	string dominionName;
	string dominionAdjective;
	map<string, string> namesByLanguage;
	map<string, string> adjectivesByLanguage;

	bool human;
};

#endif	// V2COUNTRY_H_