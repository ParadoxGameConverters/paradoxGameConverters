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
		
		
		existingLocalisation->second.insert(make_pair(destTag + "_democratic",  nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_democratic_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism", "People's Republic of " + nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism_DEF", "People's Republic of " + nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_autocratic", "Kingdom of " + nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_autocratic_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_socialist", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_socialist_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_liberal", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_liberal_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_syndicalism", "Commune of "+nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_syndicalism_DEF", nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_ancap", "Bourgeois State of " + nameInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_ancap_DEF", nameInLanguage.second));

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
		existingLocalisation->second.insert(make_pair(destTag + "_fascism", adjInLanguage.second + " Empire"));
		existingLocalisation->second.insert(make_pair(destTag + "_fascism_DEF", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_fascism_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_democratic_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_neutrality_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_communism_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_autocratic_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_socialist_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_liberal_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_syndicalism_ADJ", adjInLanguage.second));
		existingLocalisation->second.insert(make_pair(destTag + "_ancap_ADJ", adjInLanguage.second));
	}
}


void HoI4Localisation::output(string localisationPath) const
{
	outputCountries(localisationPath);
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
		localisationFile << "l_" << languageToLocalisations.first << ":\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":0 \"" << mapping.second << "\"" << endl;
		}
	}
}
