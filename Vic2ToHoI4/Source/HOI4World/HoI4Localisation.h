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



class HoI4States;
class HoI4State;
struct governmentMapping;



typedef std::map<std::string, std::string> keyToLocalisationMap; // key -> localisation
typedef std::map<std::string, keyToLocalisationMap> languageToLocalisationsMap; // language -> (key -> localisation)




class HoI4Localisation
{
	public:
		static void addStateLocalisations(const HoI4States* states)
		{
			getInstance()->AddStateLocalisations(states);
		}

		static void createCountryLocalisations(const std::pair<const std::string&, const std::string&>& tags)
		{
			getInstance()->CreateCountryLocalisations(tags);
		}

		static void addNonenglishCountryLocalisations()
		{
			getInstance()->AddNonenglishCountryLocalisations();
		}

		static void copyFocusLocalisations(const std::string& oldKey, const std::string& newKey)
		{
			getInstance()->CopyFocusLocalisations(oldKey, newKey);
		}

		static void copyEventLocalisations(const std::string& oldKey, const std::string& newKey)
		{
			getInstance()->CopyEventLocalisations(oldKey, newKey);
		}

		static void addEventLocalisation(const std::string& event, const std::string& localisation)
		{
			getInstance()->AddEventLocalisation(event, localisation);
		}

		static void addEventLocalisationFromVic2(const std::string& Vic2Key, const std::string& HoI4Key)
		{
			getInstance()->AddEventLocalisationFromVic2(Vic2Key, HoI4Key);
		}

		static void addIdeaLocalisation(const std::string& idea, const std::optional<std::string>& localisation)
		{
			getInstance()->AddIdeaLocalisation(idea, localisation);
		}

		static void addPoliticalPartyLocalisation(const std::string& Vic2Key, const std::string& HoI4Key)
		{
			getInstance()->AddPoliticalPartyLocalisation(Vic2Key, HoI4Key);
		}

		static void updateLocalisationWithCountry(const std::string& key, const std::string& oldText, const std::string& newTextLocalisationKey)
		{
			getInstance()->UpdateLocalisationWithCountry(key, oldText, newTextLocalisationKey);
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
		void importFocusLocalisations(const std::string& filename);
		void importGenericIdeaLocalisations(const std::string& filename);
		void importEventLocalisations(const std::string& filename);
		void importLocalisationFile(const std::string& filename, languageToLocalisationsMap& localisations);
		void prepareIdeaLocalisations();

		HoI4Localisation(const HoI4Localisation&) = delete;
		HoI4Localisation& operator=(const HoI4Localisation&) = delete;

		void AddNonenglishCountryLocalisations();

		void CreateCountryLocalisations(const std::pair<const std::string&, const std::string&>& tags);
		void addLocalisationsForAllGovernments(const std::pair<const std::string&, const std::string&>& tags, const std::pair<const std::string&, const std::string&>& suffixes);
		void addLocalisationsInAllLanguages(const std::string& destTag, const std::pair<const std::string&, const std::string&>& suffixes, const std::string& HoI4GovernmentIdeology, const keyToLocalisationMap& namesInLanguage);
		languageToLocalisationsMap::iterator getExistingLocalisationsInLanguage(const std::string& language);
		void addLocalisation(const std::string& newKey, languageToLocalisationsMap::iterator& existingLanguage, const std::string& localisation, const std::string& HoI4Suffix);
		bool addNeutralLocalisation(const std::pair<const std::string&, const std::string&>& tags, const std::pair<const std::string&, const std::string&>& suffixes);

		void CopyFocusLocalisations(const std::string& oldKey, const std::string& newKey);

		void CopyEventLocalisations(const std::string& oldKey, const std::string& newKey);

		void AddStateLocalisations(const HoI4States* states);
		void addStateLocalisationForLanguage(const HoI4State* state, const std::pair<const std::string, std::string>& Vic2NameInLanguage);
		void addVPLocalisationForLanguage(const HoI4State* state, const std::pair<const std::string, std::string>& Vic2NameInLanguage);
		keyToLocalisationMap& getExistingStateLocalisation(const std::string& language);
		keyToLocalisationMap& getExistingVPLocalisation(const std::string& language);
		void addLanguageToStateLocalisations(const std::string& language);
		void addLanguageToVPLocalisations(const std::string& language);
		void addNonenglishStateLocalisations();
		void addNonenglishVPLocalisations();
		void addDebugLocalisations(const std::pair<const int, HoI4State*>& state);

		void AddEventLocalisation(const std::string& event, const std::string& localisation);
		void AddEventLocalisationFromVic2(const std::string& Vic2Key, const std::string& HoI4Key);

		void AddIdeaLocalisation(const std::string& idea, const std::optional<std::string>& localisation);

		void AddPoliticalPartyLocalisation(const std::string& Vic2Key, const std::string& HoI4Key);

		void UpdateLocalisationWithCountry(const std::string& key, const std::string& oldText, const std::string& newTextLocalisationKey);

		void Output() const;
		void outputCountries(const std::string& localisationPath) const;
		void outputFocuses(const std::string& localisationPath) const;
		void outputStateLocalisations(const std::string& localisationPath) const;
		void outputVPLocalisations(const std::string& localisationPath) const;
		void outputIdeaLocalisations(const std::string& localisationPath) const;
		void outputEventLocalisations(const std::string& localisationPath) const;
		void outputPoliticalPartyLocalisations(const std::string& localisationPath) const;
		void outputLocalisations(const std::string& filenameStart, const languageToLocalisationsMap& localisations) const;

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