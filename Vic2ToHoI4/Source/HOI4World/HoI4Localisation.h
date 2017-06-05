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
#include <string>
#include <vector>
#include <map>
using namespace std;



class V2Country;
class HoI4States;
class HoI4State;



typedef std::map<std::string, std::string>				keyToLocalisationMap;			// key -> localisation
typedef std::map<std::string, keyToLocalisationMap>	languageToLocalisationsMap;	// language -> (key -> localisation)




class HoI4Localisation
{
	public:
		static void addStateLocalisations(const HoI4States* states)
		{
			getInstance()->AddStateLocalisations(states);
		}

		static void createCountryLocalisations(const string& sourceTag, const string& destTag)
		{
			getInstance()->CreateCountryLocalisations(sourceTag, destTag);
		}

		static void addNonenglishCountryLocalisations()
		{
			getInstance()->AddNonenglishCountryLocalisations();
		}

		static void copyFocusLocalisations(string oldKey, string newKey)
		{
			getInstance()->CopyFocusLocalisations(oldKey, newKey);
		}

		static void addIdeaLocalisation(const string& idea, const string& localisation)
		{
			getInstance()->AddIdeaLocalisation(idea, localisation);
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
		void importLocalisationFile(const string& filename, languageToLocalisationsMap& localisations);
		void prepareIdeaLocalisations();

		void AddNonenglishCountryLocalisations();

		void CreateCountryLocalisations(const string& sourceTag, const string& destTag);
		void CopyFocusLocalisations(string oldKey, string newKey);

		void AddStateLocalisations(const HoI4States* states);
		void addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		void addVPLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage);
		keyToLocalisationMap& getExistingStateLocalisation(const string& language);
		keyToLocalisationMap& getExistingVPLocalisation(const string& language);
		void addLanguageToStateLocalisations(const string& language);
		void addLanguageToVPLocalisations(const string& language);
		void addNonenglishStateLocalisations();
		void addNonenglishVPLocalisations();

		void AddIdeaLocalisation(const string& idea, const string& localisation);

		void Output() const;
		void outputCountries(string localisationPath) const;
		void outputFocuses(string localisationPath) const;
		void outputStateLocalisations(string localisationPath) const;
		void outputVPLocalisations(string localisationPath) const;
		void outputIdeaLocalisations(string localisationPath) const;
		void outputLocalisations(const string& filenameStart, const languageToLocalisationsMap& localisations) const;

		languageToLocalisationsMap stateLocalisations;
		languageToLocalisationsMap VPLocalisations;
		languageToLocalisationsMap countryLocalisations;
		languageToLocalisationsMap originalFocuses;
		languageToLocalisationsMap newFocuses;
		languageToLocalisationsMap ideaLocalisations;
		languageToLocalisationsMap genericIdeaLocalisations;
};



#endif // HoI4LOCALISATION_H_