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



#ifndef HoI4LOCALISATION_H_
#define HoI4LOCALISATION_H_



#include <array>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>
using namespace std;



class V2Country;
class HoI4States;
class HoI4State;
struct governmentMapping;



typedef std::map<std::string, std::string>				keyToLocalisationMap;			// key -> localisation
typedef std::map<std::string, keyToLocalisationMap>	languageToLocalisationsMap;	// language -> (key -> localisation)




class HoI4Localisation
{
	public:
		static void addStateLocalisations(const HoI4States* states)
		{
			getInstance()->AddStateLocalisations(states);
		}

		static void createCountryLocalisations(const pair<const string&, const string&>& tags)
		{
			getInstance()->CreateCountryLocalisations(tags);
		}

		static void addNonenglishCountryLocalisations()
		{
			getInstance()->AddNonenglishCountryLocalisations();
		}

		static void copyFocusLocalisations(const string& oldKey, const string& newKey)
		{
			getInstance()->CopyFocusLocalisations(oldKey, newKey);
		}

		static void copyEventLocalisations(const string& oldKey, const string& newKey)
		{
			getInstance()->CopyEventLocalisations(oldKey, newKey);
		}

		static void addEventLocalisation(const string& event, const string& localisation)
		{
			getInstance()->AddEventLocalisation(event, localisation);
		}

		static void addEventLocalisationFromVic2(const string& Vic2Key, const string& HoI4Key)
		{
			getInstance()->AddEventLocalisationFromVic2(Vic2Key, HoI4Key);
		}

		static void addIdeaLocalisation(const string& idea, const optional<string>& localisation)
		{
			getInstance()->AddIdeaLocalisation(idea, localisation);
		}

		static void addPoliticalPartyLocalisation(const string& Vic2Key, const string& HoI4Key)
		{
			getInstance()->AddPoliticalPartyLocalisation(Vic2Key, HoI4Key);
		}

		static void output()
		{
			getInstance()->Output();
		}

	private:
		static HoI4Localisation* instance;
		static HoI4Localisation* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new HoI4Localisation();
			}
			return instance;
		}
		HoI4Localisation();
		void importLocalisations();
		void importFocusLocalisations(const string& filename);
		void importGenericIdeaLocalisations(const string& filename);
		void importEventLocalisations(const string& filename);
		void importLocalisationFile(const string& filename, languageToLocalisationsMap& localisations);
		void prepareIdeaLocalisations();

		HoI4Localisation(const HoI4Localisation&) = delete;
		HoI4Localisation& operator=(const HoI4Localisation&) = delete;

		void AddNonenglishCountryLocalisations();

		void CreateCountryLocalisations(const pair<const string&, const string&>& tags);
		void addLocalisationsForAllGovernments(const pair<const string&, const string&>& tags, const pair<const string&, const string&>& suffixes);
		void addLocalisationsInAllLanguages(const string& destTag, const pair<const string&, const string&>& suffixes, const string& HoI4GovernmentIdeology, const keyToLocalisationMap& namesInLanguage);
		languageToLocalisationsMap::iterator getExistingLocalisationsInLanguage(const string& language);
		void addLocalisation(const string& newKey, languageToLocalisationsMap::iterator& existingLanguage, const string& localisation, const string& HoI4Suffix);
		bool addNeutralLocalisation(const pair<const string&, const string&>& tags, const pair<const string&, const string&>& suffixes);

		void CopyFocusLocalisations(const string& oldKey, const string& newKey);

		void CopyEventLocalisations(const string& oldKey, const string& newKey);

		void AddStateLocalisations(const HoI4States* states);
		void addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		void addVPLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		keyToLocalisationMap& getExistingStateLocalisation(const string& language);
		keyToLocalisationMap& getExistingVPLocalisation(const string& language);
		void addLanguageToStateLocalisations(const string& language);
		void addLanguageToVPLocalisations(const string& language);
		void addNonenglishStateLocalisations();
		void addNonenglishVPLocalisations();
		void addDebugLocalisations(const pair<const int, HoI4State*>& state);

		void AddEventLocalisation(const string& event, const string& localisation);
		void AddEventLocalisationFromVic2(const string& Vic2Key, const string& HoI4Key);

		void AddIdeaLocalisation(const string& idea, const optional<string>& localisation);

		void AddPoliticalPartyLocalisation(const string& Vic2Key, const string& HoI4Key);

		void Output() const;
		void outputCountries(const string& localisationPath) const;
		void outputFocuses(const string& localisationPath) const;
		void outputStateLocalisations(const string& localisationPath) const;
		void outputVPLocalisations(const string& localisationPath) const;
		void outputIdeaLocalisations(const string& localisationPath) const;
		void outputEventLocalisations(const string& localisationPath) const;
		void outputPoliticalPartyLocalisations(const string& localisationPath) const;
		void outputLocalisations(const string& filenameStart, const languageToLocalisationsMap& localisations) const;

		languageToLocalisationsMap stateLocalisations;
		languageToLocalisationsMap VPLocalisations;
		languageToLocalisationsMap countryLocalisations;
		languageToLocalisationsMap originalFocuses;
		languageToLocalisationsMap newFocuses;
		languageToLocalisationsMap ideaLocalisations;
		languageToLocalisationsMap genericIdeaLocalisations;
		languageToLocalisationsMap originalEventLocalisations;
		languageToLocalisationsMap newEventLocalisations;
		languageToLocalisationsMap politicalPartyLocalisations;
};



#endif // HoI4LOCALISATION_H_