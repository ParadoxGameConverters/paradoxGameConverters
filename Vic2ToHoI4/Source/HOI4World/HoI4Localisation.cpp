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



#include "HoI4Localisation.h"
#include <fstream>
#include "../V2World/V2Country.h"
#include "Log.h"
#include "../../../common_items/OSCompatibilityLayer.h"



void HoI4Localisation::readFromCountry(const V2Country* source, string destTag)
{
	for (auto nameInLanguage: source->getLocalisedNames())
	{
		auto existingLocalisation = countryLocalisations.find(nameInLanguage.first);
		if (existingLocalisation == countryLocalisations.end())
		{
			keyToLocalisationMap newLocalisation;
			countryLocalisations[nameInLanguage.first] = newLocalisation;
			existingLocalisation = countryLocalisations.find(nameInLanguage.first);
		}
		
		existingLocalisation->second.insert(make_pair(destTag + "_fascism", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_fascism_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_democratic", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_democratic_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism_DEF", nameInLanguage.second));
	}
	for (auto adjInLanguage: source->getLocalisedAdjectives())
	{
		auto existingLocalisation = countryLocalisations.find(adjInLanguage.first);
		if (existingLocalisation == countryLocalisations.end())
		{
			keyToLocalisationMap newLocalisation;
			countryLocalisations[adjInLanguage.first] = newLocalisation;
			existingLocalisation = countryLocalisations.find(adjInLanguage.first);
		}

		existingLocalisation->second.insert(make_pair(destTag + "_fascism_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_democratic_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism_ADJ", adjInLanguage.second));
	}
}


void HoI4Localisation::addStateLocalisation(int HoI4StateID, string Vic2StateID, const V2Localisation Vic2Localisations)
{
	for (auto nameInLanguage: Vic2Localisations.GetTextInEachLanguage(Vic2StateID))
	{
		auto existingLocalisation = stateLocalisations.find(nameInLanguage.first);
		if (existingLocalisation == stateLocalisations.end())
		{
			keyToLocalisationMap newLocalisation;
			stateLocalisations[nameInLanguage.first] = newLocalisation;
			existingLocalisation = stateLocalisations.find(nameInLanguage.first);
		}

		existingLocalisation->second.insert(make_pair(string("STATE_") + to_string(HoI4StateID), nameInLanguage.second));
	}
}


void HoI4Localisation::output(string localisationPath) const
{
	outputCountries(localisationPath);
	outputStates(localisationPath);
}


void HoI4Localisation::outputCountries(string localisationPath) const
{
	for (auto languageToLocalisations: countryLocalisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		string dest = localisationPath + "/countries_mod_l_" + languageToLocalisations.first + ".yml";
		ofstream localisationFile(localisationPath + "/countries_mod_l_" + languageToLocalisations.first + ".yml");
		if (!localisationFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not update localisation text file";
			exit(-1);
		}
		localisationFile << "\xEF\xBB\xBF"; // output a BOM to make HoI4 happy
		localisationFile << "l_" << languageToLocalisations.first << ":\r\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":0 \"" << mapping.second << "\"" << endl;
		}
	}
}


void HoI4Localisation::outputStates(string localisationPath) const
{
	for (auto languageToLocalisations: stateLocalisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		string dest = localisationPath + "/state_names_l_" + languageToLocalisations.first + ".yml";
		ofstream localisationFile(localisationPath + "/state_l_" + languageToLocalisations.first + ".yml");
		if (!localisationFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not update localisation text file";
			exit(-1);
		}
		localisationFile << "\xEF\xBB\xBF"; // output a BOM to make HoI4 happy
		localisationFile << "l_" << languageToLocalisations.first << ":\r\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":10 \"" << mapping.second << "\"" << endl;
		}
	}
}
