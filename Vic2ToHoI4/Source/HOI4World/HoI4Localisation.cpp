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
#include "HoI4State.h"
#include "HoI4States.h"
#include "../Mappers/V2Localisations.h"
#include "../V2World/V2Country.h"
#include "../V2World/Vic2State.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>



HoI4Localisation* HoI4Localisation::instance = nullptr;



HoI4Localisation::HoI4Localisation()
{
	importLocalisations();
	prepareIdeaLocalisations();
}


void HoI4Localisation::importLocalisations()
{
	set<string> filenames;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/localisation", filenames);
	for (auto filename: filenames)
	{
		if (filename.substr(0, 5) == "focus")
		{
			importFocusLocalisations(filename);
		}
		else if (filename.substr(0, 5) == "ideas")
		{
			importGenericIdeaLocalisations(filename);
		}
	}
}


void HoI4Localisation::importFocusLocalisations(const string& filename)
{
	importLocalisationFile(filename, originalFocuses);
}


void HoI4Localisation::importGenericIdeaLocalisations(const string& filename)
{
	importLocalisationFile(filename, genericIdeaLocalisations);
}


void HoI4Localisation::importLocalisationFile(const string& filename, languageToLocalisationsMap& localisations)
{
	keyToLocalisationMap newLocalisations;

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

		newLocalisations[key] = value;
	}

	localisations[language] = newLocalisations;
	file.close();
}


void HoI4Localisation::prepareIdeaLocalisations()
{
	for (auto genericLocalisationsInLanguage: genericIdeaLocalisations)
	{
		keyToLocalisationMap newLocalisationsMap;
		ideaLocalisations.insert(make_pair(genericLocalisationsInLanguage.first, newLocalisationsMap));
	}
}


void HoI4Localisation::CreateCountryLocalisations(const string& sourceTag, const string& destTag)
{
	for (auto mapping: governmentMapper::getGovernmentMappings())
	{
		auto localisationForGovernment = V2Localisations::GetTextInEachLanguage(sourceTag + "_" + mapping.vic2Government);
		for (auto nameInLanguage: localisationForGovernment)
		{
			auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
			if (existingLanguage == countryLocalisations.end())
			{
				keyToLocalisationMap newLocalisations;
				countryLocalisations[nameInLanguage.first] = newLocalisations;
				existingLanguage = countryLocalisations.find(nameInLanguage.first);
			}

			string newKey = destTag + "_" + mapping.HoI4GovernmentIdeology;
			auto existingLocalisation = existingLanguage->second.find(newKey);
			if (existingLocalisation == existingLanguage->second.end())
			{
				existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
				existingLanguage->second.insert(make_pair(newKey + "_DEF", nameInLanguage.second));
			}
		}

		if (localisationForGovernment.size() == 0)
		{
			for (auto nameInLanguage: V2Localisations::GetTextInEachLanguage(sourceTag))
			{
				auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
				if (existingLanguage == countryLocalisations.end())
				{
					keyToLocalisationMap newLocalisations;
					countryLocalisations[nameInLanguage.first] = newLocalisations;
					existingLanguage = countryLocalisations.find(nameInLanguage.first);
				}

				string newKey = destTag + "_" + mapping.HoI4GovernmentIdeology;
				auto existingLocalisation = existingLanguage->second.find(newKey);
				if (existingLocalisation == existingLanguage->second.end())
				{
					existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
					existingLanguage->second.insert(make_pair(newKey + "_DEF", nameInLanguage.second));
				}
			}
		}
	}

	for (auto mapping: governmentMapper::getGovernmentMappings())
	{
		auto localisationForGovernment = V2Localisations::GetTextInEachLanguage(sourceTag + "_" + mapping.vic2Government + "_ADJ");
		for (auto nameInLanguage: localisationForGovernment)
		{
			auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
			if (existingLanguage == countryLocalisations.end())
			{
				keyToLocalisationMap newLocalisations;
				countryLocalisations[nameInLanguage.first] = newLocalisations;
				existingLanguage = countryLocalisations.find(nameInLanguage.first);
			}

			string newKey = destTag + "_" + mapping.HoI4GovernmentIdeology + "_ADJ";
			auto existingLocalisation = existingLanguage->second.find(newKey);
			if (existingLocalisation == existingLanguage->second.end())
			{
				existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
			}
		}
		if (localisationForGovernment.size() == 0)
		{
			for (auto nameInLanguage: V2Localisations::GetTextInEachLanguage(sourceTag + "_ADJ"))
			{
				auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
				if (existingLanguage == countryLocalisations.end())
				{
					keyToLocalisationMap newLocalisations;
					countryLocalisations[nameInLanguage.first] = newLocalisations;
					existingLanguage = countryLocalisations.find(nameInLanguage.first);
				}

				string newKey = destTag + "_" + mapping.HoI4GovernmentIdeology + "_ADJ";
				auto existingLocalisation = existingLanguage->second.find(newKey);
				if (existingLocalisation == existingLanguage->second.end())
				{
					existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
				}
			}
		}
	}

	auto plainLocalisation = V2Localisations::GetTextInEachLanguage(sourceTag);
	for (auto nameInLanguage: plainLocalisation)
	{
		auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
		if (existingLanguage == countryLocalisations.end())
		{
			keyToLocalisationMap newLocalisations;
			countryLocalisations[nameInLanguage.first] = newLocalisations;
			existingLanguage = countryLocalisations.find(nameInLanguage.first);
		}

		string newKey = destTag + "_neutrality";
		auto existingLocalisation = existingLanguage->second.find(newKey);
		if (existingLocalisation == existingLanguage->second.end())
		{
			existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
			existingLanguage->second.insert(make_pair(newKey + "_DEF", nameInLanguage.second));
		}
	}
	if (plainLocalisation.size() == 0)
	{
		LOG(LogLevel::Warning) << "Could not find plain localisation for " << sourceTag;
	}

	auto plainAdjectiveLocalisation = V2Localisations::GetTextInEachLanguage(sourceTag + "_ADJ");
	for (auto nameInLanguage: plainAdjectiveLocalisation)
	{
		auto existingLanguage = countryLocalisations.find(nameInLanguage.first);
		if (existingLanguage == countryLocalisations.end())
		{
			keyToLocalisationMap newLocalisations;
			countryLocalisations[nameInLanguage.first] = newLocalisations;
			existingLanguage = countryLocalisations.find(nameInLanguage.first);
		}

		string newKey = destTag +  + "_neutrality_ADJ";
		auto existingLocalisation = existingLanguage->second.find(newKey);
		if (existingLocalisation == existingLanguage->second.end())
		{
			existingLanguage->second.insert(make_pair(newKey, nameInLanguage.second));
		}
	}
	if (plainAdjectiveLocalisation.size() == 0)
	{
		LOG(LogLevel::Warning) << "Could not find plain adjective localisation for " << sourceTag;
	}
}


void HoI4Localisation::AddNonenglishCountryLocalisations()
{
	auto englishLocalisations = countryLocalisations.find("english");
	countryLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	countryLocalisations.insert(make_pair("polish", englishLocalisations->second));
	countryLocalisations.insert(make_pair("russian", englishLocalisations->second));
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
		if (oldLocalisation != languageLocalisations.second.end())
		{
			newLanguage->second[newKey] = oldLocalisation->second;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not find original localisation for " << oldKey;
		}
	}
}


void HoI4Localisation::AddStateLocalisations(const HoI4States* states)
{
	for (auto state: states->getStates())
	{
		for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage(state.second->getSourceState()->getStateID()))
		{
			addStateLocalisationForLanguage(state.second, Vic2NameInLanguage);
		}

		int VPPositionInHoI4 = state.second->getVPLocation();
		auto VPProvinceMapping = provinceMapper::getHoI4ToVic2ProvinceMapping().find(VPPositionInHoI4);
		if (
			(VPProvinceMapping != provinceMapper::getHoI4ToVic2ProvinceMapping().end()) &&
			(VPProvinceMapping->second.size() > 0)
			)
		{
			for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage("PROV" + to_string(VPProvinceMapping->second[0])))
			{
				addVPLocalisationForLanguage(state.second, Vic2NameInLanguage);
			}
		}
	}

	addNonenglishStateLocalisations();
	addNonenglishVPLocalisations();
}


void HoI4Localisation::addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage)
{
	string key = string("STATE_") + to_string(state->getID());

	string localisedName = "";
	if (state->getSourceState()->isPartialState())
	{
		localisedName += V2Localisations::GetTextInLanguage(state->getSourceState()->getOwner() + "_ADJ", Vic2NameInLanguage.first) + " ";
	}
	localisedName += Vic2NameInLanguage.second;

	getExistingStateLocalisation(Vic2NameInLanguage.first).insert(make_pair(key, localisedName));
}


void HoI4Localisation::addVPLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage)
{
	getExistingVPLocalisation(Vic2NameInLanguage.first).insert(make_pair("VICTORY_POINTS_" + to_string(state->getVPLocation()),	Vic2NameInLanguage.second));
}


keyToLocalisationMap& HoI4Localisation::getExistingStateLocalisation(const string& language)
{
	auto existingLocalisation = stateLocalisations.find(language);
	if (existingLocalisation == stateLocalisations.end())
	{
		addLanguageToStateLocalisations(language);
		existingLocalisation = stateLocalisations.find(language);
	}

	return existingLocalisation->second;
}


keyToLocalisationMap& HoI4Localisation::getExistingVPLocalisation(const string& language)
{
	auto existingLocalisation = VPLocalisations.find(language);
	if (existingLocalisation == VPLocalisations.end())
	{
		addLanguageToVPLocalisations(language);
		existingLocalisation = VPLocalisations.find(language);
	}

	return existingLocalisation->second;
}


void HoI4Localisation::addLanguageToStateLocalisations(const string& language)
{
	keyToLocalisationMap newLocalisation;
	stateLocalisations[language] = newLocalisation;
}


void HoI4Localisation::addLanguageToVPLocalisations(const string& language)
{
	keyToLocalisationMap newLocalisation;
	VPLocalisations[language] = newLocalisation;
}


void HoI4Localisation::addNonenglishStateLocalisations()
{
	auto englishLocalisations = stateLocalisations.find("english");
	stateLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	stateLocalisations.insert(make_pair("polish", englishLocalisations->second));
	stateLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


void HoI4Localisation::addNonenglishVPLocalisations()
{
	auto englishLocalisations = VPLocalisations.find("english");
	VPLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	VPLocalisations.insert(make_pair("polish", englishLocalisations->second));
	VPLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


void HoI4Localisation::AddIdeaLocalisation(const string& idea, const string& localisation)
{
	for (auto localisationInLanguage: ideaLocalisations)
	{
		if (localisation != "")
		{
			ideaLocalisations[localisationInLanguage.first][idea] = localisation;
		}
		else
		{
			auto genericLocalisationsInLanguage = genericIdeaLocalisations.find(localisationInLanguage.first);
			if (genericLocalisationsInLanguage != genericIdeaLocalisations.end())
			{
				string genericIdeaStr = "generic" + idea.substr(3, idea.size());
				auto genericIdea = genericLocalisationsInLanguage->second.find(genericIdeaStr);
				if (genericIdea != genericLocalisationsInLanguage->second.end())
				{
					ideaLocalisations[localisationInLanguage.first][idea] = genericIdea->second;
				}
				else
				{
					LOG(LogLevel::Warning) << "Could not find localisation for " << genericIdeaStr << " in " << localisationInLanguage.first;
				}
			}
			else
			{
				LOG(LogLevel::Warning) << "No generic idea localisations found for " << localisationInLanguage.first;
			}
		}
	}
}


void HoI4Localisation::Output() const
{
	LOG(LogLevel::Debug) << "Writing localisations";
	string localisationPath = "output/" + Configuration::getOutputName() + "/localisation";
	if (!Utils::TryCreateFolder(localisationPath))
	{
		LOG(LogLevel::Error) << "Could not create localisation folder";
		exit(-1);
	}

	outputCountries(localisationPath);
	outputFocuses(localisationPath);
	outputStateLocalisations(localisationPath);
	outputVPLocalisations(localisationPath);
	outputIdeaLocalisations(localisationPath);
}


void HoI4Localisation::outputCountries(string localisationPath) const
{
	outputLocalisations(localisationPath + "/countries_mod_l_", countryLocalisations);
}


void HoI4Localisation::outputFocuses(string localisationPath) const
{
	outputLocalisations(localisationPath + "/focus_mod_l_", newFocuses);
}


void HoI4Localisation::outputStateLocalisations(string localisationPath) const
{
	outputLocalisations(localisationPath + "/state_names_l_", stateLocalisations);
}


void HoI4Localisation::outputVPLocalisations(string localisationPath) const
{
	outputLocalisations(localisationPath + "/victory_points_l_", VPLocalisations);
}


void HoI4Localisation::outputIdeaLocalisations(string localisationPath) const
{
	outputLocalisations(localisationPath + "/converted_ideas_l_", ideaLocalisations);
}


void HoI4Localisation::outputLocalisations(const string& filenameStart, const languageToLocalisationsMap& localisations) const
{
	for (auto languageToLocalisations: localisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		ofstream localisationFile(filenameStart + languageToLocalisations.first + ".yml");
		if (!localisationFile.is_open())
		{
			LOG(LogLevel::Error) << "Could not update localisation text file";
			exit(-1);
		}
		localisationFile << "\xEF\xBB\xBF"; // output a BOM to make HoI4 happy
		localisationFile << "l_" << languageToLocalisations.first << ":\n";

		for (auto mapping: languageToLocalisations.second)
		{
			localisationFile << " " << mapping.first << ":10 \"" << mapping.second << "\"" << endl;
		}
	}
}