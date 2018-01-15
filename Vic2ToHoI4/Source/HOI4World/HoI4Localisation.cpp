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



HoI4Localisation::HoI4Localisation():
	stateLocalisations(),
	VPLocalisations(),
	countryLocalisations(),
	originalFocuses(),
	newFocuses(),
	ideaLocalisations(),
	genericIdeaLocalisations(),
	originalEventLocalisations(),
	newEventLocalisations(),
	politicalPartyLocalisations()
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
			importFocusLocalisations(Configuration::getHoI4Path() + "/localisation/" + filename);
		}
		else if (filename.substr(0, 5) == "ideas")
		{
			importGenericIdeaLocalisations(Configuration::getHoI4Path() + "/localisation/" + filename);
		}
		else if (filename.substr(0, 6) == "events")
		{
			importEventLocalisations(Configuration::getHoI4Path() + "/localisation/" + filename);
		}
	}

	filenames.clear();
	Utils::GetAllFilesInFolder("blankmod/output/localisation", filenames);
	for (auto filename: filenames)
	{
		if (filename.substr(0, 5) == "focus")
		{
			importFocusLocalisations("blankmod/output/localisation/" + filename);
		}
		else if (filename.substr(0, 5) == "ideas")
		{
			importGenericIdeaLocalisations("blankmod/output/localisation/" + filename);
		}
		else if (filename.substr(0, 6) == "events")
		{
			importEventLocalisations("blankmod/output/localisation/" + filename);
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


void HoI4Localisation::importEventLocalisations(const string& filename)
{
	importLocalisationFile(filename, originalEventLocalisations);
}


void HoI4Localisation::importLocalisationFile(const string& filename, languageToLocalisationsMap& localisations)
{
	keyToLocalisationMap newLocalisations;

	ifstream file(filename);
	if (!file.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << filename;
		exit(-1);
	}
	char bitBucket[3];
	file.read(bitBucket, 3);

	string language;
	while (!file.eof())
	{
		char buffer[2048];
		file.getline(buffer, sizeof(buffer));
		string line(buffer);
		if (line.substr(0,2) == "l_")
		{
			language = line.substr(2, line.length() - 3);
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

	auto localisationsInLanguage = localisations.find(language);
	if (localisationsInLanguage == localisations.end())
	{
		localisations[language] = newLocalisations;
	}
	else
	{
		for (auto localisation: newLocalisations)
		{
			localisationsInLanguage->second.insert(localisation);
		}
	}

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


void HoI4Localisation::CreateCountryLocalisations(const pair<const string&, const string&>& tags)
{
	addLocalisationsForAllGovernments(tags, make_pair(string(""), string("_DEF")));
	addLocalisationsForAllGovernments(tags, make_pair(string("_ADJ"), string("")));

	if (!addNeutralLocalisation(tags, make_pair(string(""), string("_DEF"))))
	{
		LOG(LogLevel::Warning) << "Could not find plain localisation for " << tags.first;
	}
	if (!addNeutralLocalisation(tags, make_pair(string("_ADJ"), string(""))))
	{
		LOG(LogLevel::Warning) << "Could not find plain adjective localisation for " << tags.first;
	}
}


void HoI4Localisation::addLocalisationsForAllGovernments(const pair<const string&, const string&>& tags, const pair<const string&, const string&>& suffixes)
{
	for (auto mapping: governmentMapper::getGovernmentMappings())
	{
		auto localisationForGovernment = V2Localisations::GetTextInEachLanguage(tags.first + "_" + mapping.vic2Government + suffixes.first);
		addLocalisationsInAllLanguages(tags.second, suffixes, mapping.HoI4GovernmentIdeology, localisationForGovernment);
		if (localisationForGovernment.size() == 0)
		{
			addLocalisationsInAllLanguages(tags.second, suffixes, mapping.HoI4GovernmentIdeology, V2Localisations::GetTextInEachLanguage(tags.first + suffixes.first));
		}
	}
}


void HoI4Localisation::addLocalisationsInAllLanguages(const string& destTag, const pair<const string&, const string&>& suffixes, const string& HoI4GovernmentIdeology, const keyToLocalisationMap& namesInLanguage)
{
	for (auto nameInLanguage: namesInLanguage)
	{
		auto existingLanguage = getExistingLocalisationsInLanguage(nameInLanguage.first);

		string newKey = destTag + "_" + HoI4GovernmentIdeology + suffixes.first;
		addLocalisation(newKey, existingLanguage, nameInLanguage.second, suffixes.second);
	}
}


bool HoI4Localisation::addNeutralLocalisation(const pair<const string&, const string&>& tags, const pair<const string&, const string&>& suffixes)
{
	auto plainLocalisation = V2Localisations::GetTextInEachLanguage(tags.first + suffixes.first);
	if (plainLocalisation.size() > 0)
	{
		for (auto nameInLanguage: plainLocalisation)
		{
			auto existingLanguage = getExistingLocalisationsInLanguage(nameInLanguage.first);

			string newKey = tags.second + "_neutrality" + suffixes.first;
			addLocalisation(newKey, existingLanguage, nameInLanguage.second, suffixes.second);
		}
		return true;
	}
	else
	{
		return false;
	}
}


languageToLocalisationsMap::iterator HoI4Localisation::getExistingLocalisationsInLanguage(const string& language)
{
	auto existingLanguage = countryLocalisations.find(language);
	if (existingLanguage == countryLocalisations.end())
	{
		keyToLocalisationMap newLocalisations;
		countryLocalisations[language] = newLocalisations;
		existingLanguage = countryLocalisations.find(language);
	}

	return existingLanguage;
}


void HoI4Localisation::addLocalisation(const string& newKey, languageToLocalisationsMap::iterator& existingLanguage, const string& localisation, const string& HoI4Suffix)
{
	auto existingLocalisation = existingLanguage->second.find(newKey);
	if (existingLocalisation == existingLanguage->second.end())
	{
		existingLanguage->second.insert(make_pair(newKey, localisation));
		if (HoI4Suffix != "")
		{
			existingLanguage->second.insert(make_pair(newKey + HoI4Suffix, localisation));
		}
	}
	else
	{
		existingLanguage->second[newKey] = localisation;
		if (HoI4Suffix != "")
		{
			existingLanguage->second[newKey + HoI4Suffix] = localisation;
		}
	}
}


void HoI4Localisation::AddNonenglishCountryLocalisations()
{
	auto englishLocalisations = countryLocalisations.find("english");
	countryLocalisations.insert(make_pair("braz_por", englishLocalisations->second));
	countryLocalisations.insert(make_pair("polish", englishLocalisations->second));
	countryLocalisations.insert(make_pair("russian", englishLocalisations->second));
}


void HoI4Localisation::CopyFocusLocalisations(const string& oldKey, const string& newKey)
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
			LOG(LogLevel::Warning) << "Could not find original localisation for " << oldKey << " in " << languageLocalisations.first;
		}
	}
}


void HoI4Localisation::CopyEventLocalisations(const string& oldKey, const string& newKey)
{
	for (auto languageLocalisations: originalEventLocalisations)
	{
		auto newLanguage = newEventLocalisations.find(languageLocalisations.first);
		if (newLanguage == newEventLocalisations.end())
		{
			keyToLocalisationMap newLocalisations;
			newEventLocalisations.insert(make_pair(languageLocalisations.first, newLocalisations));
			newLanguage = newEventLocalisations.find(languageLocalisations.first);
		}

		auto oldLocalisation = languageLocalisations.second.find(oldKey);
		if (oldLocalisation != languageLocalisations.second.end())
		{
			newLanguage->second[newKey] = oldLocalisation->second;
		}
		else
		{
			LOG(LogLevel::Warning) << "Could not find original localisation for " << oldKey << " in " << languageLocalisations.first;
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

		if (Configuration::getDebug())
		{
			addDebugLocalisations(state);
		}
	}

	addNonenglishStateLocalisations();
	addNonenglishVPLocalisations();
}


void HoI4Localisation::addDebugLocalisations(const pair<const int, HoI4State*>& state)
{
	for (auto VPPositionInHoI4: state.second->getDebugVPs())
	{
		auto VPProvinceMapping = provinceMapper::getHoI4ToVic2ProvinceMapping().find(VPPositionInHoI4);
		if (
				(VPProvinceMapping != provinceMapper::getHoI4ToVic2ProvinceMapping().end()) &&
				(VPProvinceMapping->second.size() > 0)
			)
		{
			for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage("PROV" + to_string(VPProvinceMapping->second[0])))
			{
				getExistingVPLocalisation(Vic2NameInLanguage.first).insert(make_pair("VICTORY_POINTS_" + to_string(VPPositionInHoI4), Vic2NameInLanguage.second));
			}
		}
	}

	for (auto VPPositionInHoI4: state.second->getSecondaryDebugVPs())
	{
		auto VPProvinceMapping = provinceMapper::getHoI4ToVic2ProvinceMapping().find(VPPositionInHoI4);
		if (
			(VPProvinceMapping != provinceMapper::getHoI4ToVic2ProvinceMapping().end()) &&
			(VPProvinceMapping->second.size() > 0)
			)
		{
			for (auto Vic2NameInLanguage: V2Localisations::GetTextInEachLanguage("PROV" + to_string(VPProvinceMapping->second[0])))
			{
				getExistingVPLocalisation(Vic2NameInLanguage.first).insert(make_pair("VICTORY_POINTS_" + to_string(VPPositionInHoI4), "_" + Vic2NameInLanguage.second));
			}
		}
	}
}


void HoI4Localisation::addStateLocalisationForLanguage(const HoI4State* state, const pair<const string, string>& Vic2NameInLanguage)
{
	string key = string("STATE_") + to_string(state->getID());

	string localisedName = "";
	if (state->getSourceState()->isPartialState())
	{
		auto possibleOwnerAdjective = V2Localisations::GetTextInLanguage(state->getSourceState()->getOwner() + "_ADJ", Vic2NameInLanguage.first);
		if (possibleOwnerAdjective)
		{
			localisedName += *possibleOwnerAdjective + " ";
		}
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


void HoI4Localisation::AddEventLocalisation(const string& event, const string& localisation)
{
	for (auto localisationInLanguage: newEventLocalisations)
	{
		newEventLocalisations[localisationInLanguage.first][event] = localisation;
	}
}


void HoI4Localisation::AddEventLocalisationFromVic2(const string& Vic2Key, const string& HoI4Key)
{
	for (auto textInLanguage: V2Localisations::GetTextInEachLanguage(Vic2Key))
	{
		auto language = textInLanguage.first;
		auto existingLanguage = newEventLocalisations.find(language);
		if (existingLanguage == newEventLocalisations.end())
		{
			keyToLocalisationMap mappings;
			newEventLocalisations.insert(make_pair(language, mappings));
			existingLanguage = newEventLocalisations.find(language);
		}

		existingLanguage->second[HoI4Key] = textInLanguage.second;
	}
}


void HoI4Localisation::AddIdeaLocalisation(const string& idea, const optional<string>& localisation)
{
	for (auto localisationInLanguage: ideaLocalisations)
	{
		if (localisation)
		{
			ideaLocalisations[localisationInLanguage.first][idea] = *localisation;
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


void HoI4Localisation::AddPoliticalPartyLocalisation(const string& Vic2Key, const string& HoI4Key)
{
	for (auto textInLanguage: V2Localisations::GetTextInEachLanguage(Vic2Key))
	{
		auto language = textInLanguage.first;
		auto existingLanguage = politicalPartyLocalisations.find(language);
		if (existingLanguage == politicalPartyLocalisations.end())
		{
			keyToLocalisationMap mappings;
			politicalPartyLocalisations.insert(make_pair(language, mappings));
			existingLanguage = politicalPartyLocalisations.find(language);
		}

		existingLanguage->second[HoI4Key] = textInLanguage.second;
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
	outputEventLocalisations(localisationPath);
	outputPoliticalPartyLocalisations(localisationPath);
}


void HoI4Localisation::outputCountries(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/countries_mod_l_", countryLocalisations);
}


void HoI4Localisation::outputFocuses(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/focus_mod_l_", newFocuses);
}


void HoI4Localisation::outputStateLocalisations(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/state_names_l_", stateLocalisations);
}


void HoI4Localisation::outputVPLocalisations(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/victory_points_l_", VPLocalisations);
}


void HoI4Localisation::outputIdeaLocalisations(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/converted_ideas_l_", ideaLocalisations);
}


void HoI4Localisation::outputEventLocalisations(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/converted_events_l_", newEventLocalisations);
}


void HoI4Localisation::outputPoliticalPartyLocalisations(const string& localisationPath) const
{
	outputLocalisations(localisationPath + "/parties3_l_", politicalPartyLocalisations);
}


void HoI4Localisation::outputLocalisations(const string& filenameStart, const languageToLocalisationsMap& localisations) const
{
	for (auto languageToLocalisations: localisations)
	{
		if (languageToLocalisations.first == "")
		{
			continue;
		}
		ofstream localisationFile(filenameStart + languageToLocalisations.first + ".yml", ios_base::app);
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