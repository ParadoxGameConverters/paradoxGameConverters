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



#include "HoI4Localisation.h"
#include <fstream>
#include "../V2World/V2Country.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



HoI4Localisation* HoI4Localisation::instance = nullptr;



HoI4Localisation::HoI4Localisation()
{
	importFocusLocalisations();
}


void HoI4Localisation::ReadFromCountry(const V2Country* source, string destTag)
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


void HoI4Localisation::AddNonenglishCountryLocalisations()
{
	auto englishLocalisations = countryLocalisations.find("english");
	countryLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	countryLocalisations.insert(make_pair("polish", englishLocalisations->second));
	countryLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


void HoI4Localisation::importFocusLocalisations()
{
	set<string> filenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/localisation", filenames);
	for (auto filename: filenames)
	{
		if (filename.substr(0, 5) == "focus")
		{
			keyToLocalisationMap localisations;

			int period = filename.find('.');
			string language = filename.substr(8, period - 8);

			ifstream file(Configuration::getHoI4Path() + "/localisation/" + filename);
			char bitBucket[3];
			file.read(bitBucket, 3);

			while (!file.eof())
			{
				char buffer[1024];
				file.getline(buffer, sizeof(buffer));
				string line(buffer);
				if (line.substr(0,2) == "l_")
				{
					continue;
				}

				int colon = line.find(':');
				if (colon == string::npos)
				{
					continue;
				}
				string key = line.substr(1, colon - 1);

				line = line.substr(colon, line.length());
				int quote = line.find('\"');
				string value = line.substr(quote + 1, (line.length() - quote - 2));

				localisations[key] = value;
			}

			originalFocuses[language] = localisations;
			file.close();
		}
	}
}


void HoI4Localisation::CopyFocusLocalisations(string oldKey, string newKey)
{
	for (auto languageLocalisations: originalFocuses)
	{
		auto newLanguage = newFocuses.find(languageLocalisations.first);
		if (newLanguage == newFocuses.end())
		{
			keyToLocalisationMap newLocalisations;
			newFocuses.insert(make_pair(languageLocalisations.first, newLocalisations));
			newLanguage = newFocuses.find(languageLocalisations.first);
		}

		auto oldLocalisation = languageLocalisations.second.find(oldKey);
		newLanguage->second[newKey] = oldLocalisation->second;

	}
}


void HoI4Localisation::Output() const
{
	LOG(LogLevel::Debug) << "Writing localisations";
	string localisationPath = "Output/" + Configuration::getOutputName() + "/localisation";
	if (!Utils::TryCreateFolder(localisationPath))
	{
		LOG(LogLevel::Error) << "Could not create localisation folder";
		exit(-1);
	}

	outputCountries(localisationPath);
	outputFocuses(localisationPath);
}


void HoI4Localisation::outputCountries(string localisationPath) const
{
	for (auto languageToLocalisations: countryLocalisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}

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


void HoI4Localisation::outputFocuses(string localisationPath) const
{
	for (auto languageToLocalisations: newFocuses)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}

		ofstream localisationFile(localisationPath + "/focus_mod_l_" + languageToLocalisations.first + ".yml");
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
