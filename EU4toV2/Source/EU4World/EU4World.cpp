/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "EU4World.h"
#include <algorithm>
#include <fstream>
#include <string>
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "../Configuration.h"
#include "../Mappers/CultureMapper.h"
#include "../Mappers/ProvinceMapper.h"
#include "../Mappers/ReligionMapper.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include "Countries.h"
#include "CultureGroups.h"
#include "EU4Province.h"
#include "EU4Country.h"
#include "EU4Diplomacy.h"
#include "EU4Version.h"
#include "EU4Localisation.h"
#include "EU4Religion.h"
#include <set>



EU4::world::world(const string& EU4SaveFileName):
	theCountries()
{
	registerKeyword(std::regex("EU4txt"), [this](const std::string& unused, std::istream& theStream){});
	registerKeyword(std::regex("date"), [this](const std::string& dateText, std::istream& theStream)
		{
			commonItems::singleString dateString(theStream);
			date endDate(dateString.getString());
			Configuration::setLastEU4Date(endDate);
		}
	);
	registerKeyword(std::regex("save_game"), commonItems::ignoreString);
	registerKeyword(std::regex("player"), commonItems::ignoreString);
	registerKeyword(std::regex("colonial_father"), commonItems::ignoreString);
	registerKeyword(std::regex("colonial_flag_color"), commonItems::ignoreObject);
	registerKeyword(std::regex("country_colors"), commonItems::ignoreObject);
	registerKeyword(std::regex("displayed_country_name"), commonItems::ignoreString);
	registerKeyword(std::regex("savegame_version"), [this](const std::string& versionText, std::istream& theStream)
		{
			auto versionObject = commonItems::convert8859Object(versionText, theStream);
			loadEU4Version(versionObject);
		}
	);
	registerKeyword(std::regex("savegame_versions"), commonItems::ignoreObject);
	registerKeyword(std::regex("dlc_enabled"), [this](const std::string& DLCText, std::istream& theStream)
		{
			auto versionsObject = commonItems::convert8859Object(DLCText, theStream);
			loadActiveDLC(versionsObject);
		}
	);
	registerKeyword(std::regex("mod_enabled"), [this](const std::string& modText, std::istream& theStream)
		{
			auto modsObject = commonItems::convert8859Object(modText, theStream);
			loadUsedMods(modsObject);
		}
	);
	registerKeyword(std::regex("multi_player"), commonItems::ignoreString);
	registerKeyword(std::regex("not_observer"), commonItems::ignoreString);
	registerKeyword(std::regex("players_countries"), commonItems::ignoreObject);
	registerKeyword(std::regex("campaign_id"), commonItems::ignoreString);
	registerKeyword(std::regex("campaign_length"), commonItems::ignoreString);
	registerKeyword(std::regex("campaign_stats"), commonItems::ignoreObject);
	registerKeyword(std::regex("gameplaysettings"), commonItems::ignoreObject);
	registerKeyword(std::regex("used_client_names"), commonItems::ignoreObject);
	registerKeyword(std::regex("speed"), commonItems::ignoreString);
	registerKeyword(std::regex("ignore_end_date"), commonItems::ignoreString);
	registerKeyword(std::regex("multiplayer_random_seed"), commonItems::ignoreString);
	registerKeyword(std::regex("multiplayer_random_count"), commonItems::ignoreString);
	registerKeyword(std::regex("monarch"), commonItems::ignoreString);
	registerKeyword(std::regex("leader"), commonItems::ignoreString);
	registerKeyword(std::regex("advisor"), commonItems::ignoreString);
	registerKeyword(std::regex("rebel"), commonItems::ignoreString);
	registerKeyword(std::regex("current_age"), commonItems::ignoreString);
	registerKeyword(std::regex("next_age_progress"), commonItems::ignoreString);
	registerKeyword(std::regex("id_counters"), commonItems::ignoreObject);
	registerKeyword(std::regex("unit"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_template_id"), commonItems::ignoreString);
	registerKeyword(std::regex("flags"), commonItems::ignoreObject);
	registerKeyword(std::regex("revolution_target"), [this](const std::string& revolutionText, std::istream& theStream)
		{
			auto modsObject = commonItems::convert8859String(revolutionText, theStream);
			loadRevolutionTargetString(modsObject);
		}
	);
	registerKeyword(std::regex("revolution_target_original_name"), commonItems::ignoreString);
	registerKeyword(std::regex("has_first_revolution_started"), commonItems::ignoreString);
	registerKeyword(std::regex("start_date"), commonItems::ignoreString);
	registerKeyword(std::regex("map_area_data"), commonItems::ignoreObject);
	registerKeyword(std::regex("total_military_power"), commonItems::ignoreString);
	registerKeyword(std::regex("average_military_power"), commonItems::ignoreString);
	registerKeyword(std::regex("institution_origin"), commonItems::ignoreObject);
	registerKeyword(std::regex("institutions"), commonItems::ignoreObject);
	registerKeyword(std::regex("institutions_penalties"), commonItems::ignoreObject);
	registerKeyword(std::regex("trade"), commonItems::ignoreObject);
	registerKeyword(std::regex("unit_templates"), commonItems::ignoreObject);
	registerKeyword(std::regex("production_leader_tag"), commonItems::ignoreObject);
	registerKeyword(std::regex("dynamic_countries"), commonItems::ignoreObject);
	registerKeyword(std::regex("tradegoods_total_produced"), commonItems::ignoreObject);
	registerKeyword(std::regex("change_price"), commonItems::ignoreObject);
	registerKeyword(std::regex("id"), commonItems::ignoreObject);
	registerKeyword(std::regex("dynasty"), commonItems::ignoreObject);
	registerKeyword(std::regex("rebel_faction"), commonItems::ignoreObject);
	registerKeyword(std::regex("continent"), commonItems::ignoreString);
	registerKeyword(std::regex("imperial_ban_allowed"), commonItems::ignoreString);
	registerKeyword(std::regex("internal_hre_cb"), commonItems::ignoreString);
	registerKeyword(std::regex("hre_inheritable"), commonItems::ignoreString);
	registerKeyword(std::regex("allows_female_emperor"), commonItems::ignoreString);
	registerKeyword(std::regex("hre_leagues_status"), commonItems::ignoreString);
	registerKeyword(std::regex("hre_religion_status"), commonItems::ignoreString);
	registerKeyword(std::regex("old_emperor"), commonItems::ignoreObject);
	registerKeyword(std::regex("great_powers"), commonItems::ignoreObject);
	registerKeyword(std::regex("empire"), [this](const std::string& empireText, std::istream& theStream)
		{
			auto empireObject = commonItems::convert8859Object(empireText, theStream);
			loadEmpires(empireObject);
		}
	);
	registerKeyword(std::regex("emperor"), [this](const std::string& emperorText, std::istream& theStream)
		{
			auto emperorObject = commonItems::convert8859Object(emperorText, theStream);
			loadEmpires(emperorObject);
		}
	);
	registerKeyword(std::regex("celestial_empire"), [this](const std::string& empireText, std::istream& theStream)
		{
			auto empireObject = commonItems::convert8859Object(empireText, theStream);
			loadEmpires(empireObject);
		}
	);
	registerKeyword(std::regex("hre_leagues_status"), commonItems::ignoreString);
	registerKeyword(std::regex("hre_religion_status"), commonItems::ignoreString);
	registerKeyword(std::regex("religions"), commonItems::ignoreObject);
	registerKeyword(std::regex("religion_instance_data"), commonItems::ignoreObject);
	registerKeyword(std::regex("fired_events"), commonItems::ignoreObject);
	registerKeyword(std::regex("pending_events"), commonItems::ignoreObject);
	registerKeyword(std::regex("provinces"), [this](const std::string& provincesText, std::istream& theStream)
		{
			auto provincesObject = commonItems::convert8859Object(provincesText, theStream);
			loadProvinces(provincesObject);
		}
	);
	registerKeyword(std::regex("countries"), [this](const std::string& countriesText, std::istream& theStream) { loadCountries(theStream);	} );
	registerKeyword(std::regex("active_advisors"), commonItems::ignoreObject);
	registerKeyword(std::regex("diplomacy"), [this](const std::string& diplomacyText, std::istream& theStream)
		{
			auto diplomacyObject = commonItems::convert8859Object(diplomacyText, theStream);
			loadDiplomacy(diplomacyObject);
		}
	);
	registerKeyword(std::regex("combat"), commonItems::ignoreObject);
	registerKeyword(std::regex("active_war"), commonItems::ignoreObject);
	registerKeyword(std::regex("previous_war"), commonItems::ignoreObject);
	registerKeyword(std::regex("income_statistics"), commonItems::ignoreObject);
	registerKeyword(std::regex("nation_size_statistics"), commonItems::ignoreObject);
	registerKeyword(std::regex("score_statistics"), commonItems::ignoreObject);
	registerKeyword(std::regex("inflation_statistics"), commonItems::ignoreObject);
	registerKeyword(std::regex("expanded_dip_action_groups"), commonItems::ignoreObject);
	registerKeyword(std::regex("achievement_ok"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_manager"), commonItems::ignoreObject);
	registerKeyword(std::regex("trade_company_manager"), commonItems::ignoreObject);
	registerKeyword(std::regex("ai"), commonItems::ignoreObject);
	registerKeyword(std::regex("checksum"), commonItems::ignoreString);

	LOG(LogLevel::Info) << "* Importing EU4 save *";
	verifySave(EU4SaveFileName);
	parseFile(EU4SaveFileName);

	LOG(LogLevel::Info) << "Building world";
	setEmpires();
	addProvinceInfoToCountries();
	determineProvinceWeights();
	checkAllEU4CulturesMapped();
	readCommonCountries();
	setLocalisations();
	resolveRegimentTypes();
	mergeNations();
	checkAllProvincesMapped();
	setNumbersOfDestinationProvinces();
	loadRevolutionTarget();

	EU4Religion::createSelf();
	checkAllEU4ReligionsMapped();

	removeEmptyNations();
	if (Configuration::getRemovetype() == "dead")
	{
		removeDeadLandlessNations();
	}
	else if (Configuration::getRemovetype() == "all")
	{
		removeLandlessNations();
	}
}


void EU4::world::verifySave(const string& EU4SaveFileName)
{
	ifstream saveFile(EU4SaveFileName);
	if (!saveFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open save! Exiting!";
		exit(-1);
	}
	else
	{
		char buffer[7];
		saveFile.get(buffer, 7);
		if ((buffer[0] == 'P') && (buffer[1] == 'K'))
		{
			LOG(LogLevel::Error) << "Saves must be uncompressed to be converted.";
			exit(-1);
		}
		else if ((buffer[0] = 'E') && (buffer[1] == 'U') && (buffer[2] == '4') && (buffer[3] = 'b') && (buffer[4] == 'i') && (buffer[5] == 'n') && (buffer[6] == 'M'))
		{
			LOG(LogLevel::Error) << "Ironman saves cannot be converted.";
			exit(-1);
		}
	}
}


void EU4::world::loadUsedMods(const shared_ptr<Object> EU4SaveObj)
{
	LOG(LogLevel::Debug) << "Get EU4 Mods";
	map<string, string> possibleMods = loadPossibleMods();

	vector<shared_ptr<Object>> modObj = EU4SaveObj->getValue("mod_enabled");	// the used mods
	if (modObj.size() > 0)
	{
		string modString = modObj[0]->getLeaf();	// the names of all the mods
		while (modString != "")
		{
			string newMod;	// the corrected name of the mod
			const int firstQuote = modString.find("\"");	// the location of the first quote, defining the start of a mod name
			if (firstQuote == std::string::npos)
			{
				newMod.clear();
				modString.clear();
			}
			else
			{
				const int secondQuote = modString.find("\"", firstQuote + 1);	// the location of the second quote, defining the end of a mod name
				if (secondQuote == std::string::npos)
				{
					newMod.clear();
					modString.clear();
				}
				else
				{
					newMod = modString.substr(firstQuote + 1, secondQuote - firstQuote - 1);
					modString = modString.substr(secondQuote + 1, modString.size());
				}
			}

			if (newMod != "")
			{
				map<string, string>::iterator modItr = possibleMods.find(newMod);
				if (modItr != possibleMods.end())
				{
					string newModPath = modItr->second;	// the path for this mod
					if (!Utils::doesFolderExist(newModPath) && !Utils::DoesFileExist(newModPath))
					{
						LOG(LogLevel::Error) << newMod << " could not be found in the specified mod directory - a valid mod directory must be specified. Tried " << newModPath;
						exit(-1);
					}
					else
					{
						LOG(LogLevel::Debug) << "EU4 Mod is at " << newModPath;
						Configuration::addEU4Mod(newModPath);
					}
				}
				else
				{
					LOG(LogLevel::Error) << "No path could be found for " << newMod;
					exit(-1);
				}
			}
		}
	}
}


map<string, string> EU4::world::loadPossibleMods()
{
	map<string, string> possibleMods;
	loadEU4ModDirectory(possibleMods);
	loadCK2ExportDirectory(possibleMods);

	return possibleMods;
}


void EU4::world::loadEU4ModDirectory(map<string, string>& possibleMods)
{
	LOG(LogLevel::Debug) << "Get EU4 Mod Directory";
	string EU4DocumentsLoc = Configuration::getEU4DocumentsPath();	// the EU4 My Documents location as stated in the configuration file
	if (Utils::DoesFileExist(EU4DocumentsLoc))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 4 documents directory was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU4 Documents directory is " << EU4DocumentsLoc;
		set<string> fileNames;
		Utils::GetAllFilesInFolder(EU4DocumentsLoc + "/mod", fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			const int pos = itr->find_last_of('.');	// the position of the last period in the filename
			if (itr->substr(pos, itr->length()) == ".mod")
			{
				shared_ptr<Object> modObj = parser_UTF8::doParseFile((EU4DocumentsLoc + "/mod/" + *itr).c_str());	// the parsed mod file

				string name;	// the name of the mod
				vector<shared_ptr<Object>> nameObj = modObj->getValue("name");
				if (nameObj.size() > 0)
				{
					name = nameObj[0]->getLeaf();
				}
				else
				{
					name = "";
				}

				string path;	// the path of the mod
				vector<shared_ptr<Object>> dirObjs = modObj->getValue("path");	// the possible paths of the mod
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<shared_ptr<Object>> dirObjs = modObj->getValue("archive");	// the other possible paths of the mod (if its zipped)
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if ((name != "") && (path != ""))
				{
					possibleMods.insert(make_pair(name, EU4DocumentsLoc + "/" + path));
					Log(LogLevel::Debug) << "\t\tFound a mod named " << name << " claiming to be at " << EU4DocumentsLoc << "/" << path;
				}
			}
		}
	}
}


void EU4::world::loadCK2ExportDirectory(map<string, string>& possibleMods)
{
	LOG(LogLevel::Debug) << "Get CK2 Export Directory";
	string CK2ExportLoc = Configuration::getCK2ExportPath();		// the CK2 converted mods location as stated in the configuration file
	if (Utils::DoesFileExist(CK2ExportLoc))
	{
		LOG(LogLevel::Warning) << "No Crusader Kings 2 mod directory was specified in configuration.txt, or the path was invalid - this will cause problems with CK2 converted saves";
	}
	else
	{
		LOG(LogLevel::Debug) << "CK2 export directory is " << CK2ExportLoc;
		set<string> fileNames;
		Utils::GetAllFilesInFolder(CK2ExportLoc, fileNames);
		for (set<string>::iterator itr = fileNames.begin(); itr != fileNames.end(); itr++)
		{
			const int pos = itr->find_last_of('.');	// the last period in the filename
			if ((pos != string::npos) && (itr->substr(pos, itr->length()) == ".mod"))
			{
				shared_ptr<Object> modObj = parser_UTF8::doParseFile((CK2ExportLoc + "/" + *itr).c_str());	// the parsed mod file
				vector<shared_ptr<Object>> nameObj = modObj->getValue("name");
				string name;
				if (nameObj.size() > 0)
				{
					name = nameObj[0]->getLeaf();
				}

				string path;	// the path of the mod
				vector<shared_ptr<Object>> dirObjs = modObj->getValue("user_dir");	// the possible paths for the mod
				if (dirObjs.size() > 0)
				{
					path = dirObjs[0]->getLeaf();
				}
				else
				{
					vector<shared_ptr<Object>> dirObjs = modObj->getValue("archive");	// the other possible paths for the mod (if it's zipped)
					if (dirObjs.size() > 0)
					{
						path = dirObjs[0]->getLeaf();
					}
				}

				if (path != "")
				{
					possibleMods.insert(make_pair(name, CK2ExportLoc + "/" + path));
				}
			}
		}
	}
}


void EU4::world::loadEU4Version(const shared_ptr<Object> EU4SaveObj)
{
	vector<shared_ptr<Object>> versionObj = EU4SaveObj->getValue("savegame_version");
	(versionObj.size() > 0) ? version = new EU4Version(versionObj[0]) : version = new EU4Version();
	Configuration::setEU4Version(*version);
}


void EU4::world::loadActiveDLC(const shared_ptr<Object> EU4SaveObj)
{
	vector<shared_ptr<Object>> enabledDLCsObj = EU4SaveObj->getValue("dlc_enabled");
	if (enabledDLCsObj.size() > 0)
	{
		vector<string>	activeDLCs;
		vector<string> DLCsObj = enabledDLCsObj[0]->getTokens();
		for (auto DLCsItr: DLCsObj)
		{
			activeDLCs.push_back(DLCsItr);
		}

		Configuration::setActiveDLCs(activeDLCs);
	}
}


void EU4::world::loadEmpires(const shared_ptr<Object> EU4SaveObj)
{
	vector<shared_ptr<Object>> emperorObj = EU4SaveObj->getValue("emperor");
	if (emperorObj.size() > 0)
	{
		holyRomanEmperor = emperorObj[0]->getLeaf();
	}

	vector<shared_ptr<Object>> empireObj = EU4SaveObj->getValue("empire");
	if (empireObj.size() > 0)
	{
		loadHolyRomanEmperor(empireObj);
	}
	vector<shared_ptr<Object>> celestialEmpireObj = EU4SaveObj->getValue("celestial_empire");
	if (celestialEmpireObj.size() > 0)
	{
		loadCelestialEmperor(celestialEmpireObj);
	}
}

void EU4::world::loadHolyRomanEmperor(vector<shared_ptr<Object>> empireObj)
{
	vector<shared_ptr<Object>> emperorObj = empireObj[0]->getValue("emperor");
	if (emperorObj.size() > 0)
	{
		holyRomanEmperor = emperorObj[0]->getLeaf();
	}
}


void EU4::world::loadCelestialEmperor(vector<shared_ptr<Object>> celestialEmpireObj)
{
	vector<shared_ptr<Object>> emperorObj = celestialEmpireObj[0]->getValue("emperor");
	if (emperorObj.size() > 0)
	{
		celestialEmperor = emperorObj[0]->getLeaf();
	}
}

void EU4::world::loadProvinces(const shared_ptr<Object> EU4SaveObj)
{
	auto validProvinces = determineValidProvinces();

	provinces.clear();
	vector<shared_ptr<Object>> provincesObj = EU4SaveObj->getValue("provinces");					// the object holding the provinces
	if (provincesObj.size() > 0)
	{
		vector<shared_ptr<Object>> provincesLeaves = provincesObj[0]->getLeaves();		// the objects holding the individual provinces
		for (unsigned int j = 0; j < provincesLeaves.size(); j++)
		{
			string keyProv = (provincesLeaves[j])->getKey();						// the key for the province

			if (
				(atoi(keyProv.c_str()) < 0) &&													// Check if key is a negative value (EU4 style)
				(validProvinces.find(-1 * atoi(keyProv.c_str())) != validProvinces.end())	// check it's a valid province for this version of EU4
				)
			{
				EU4Province* province = new EU4Province((provincesLeaves[j]));	// the province in our format
				provinces.insert(make_pair(province->getNum(), province));
			}
		}
	}
}


map<int, int> EU4::world::determineValidProvinces()
{
	// Use map/definition.csv to determine valid provinces
	map<int, int> validProvinces;
	ifstream definitionFile((Configuration::getEU4Path() + "/map/definition.csv").c_str());
	if (!definitionFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not open map/definition.csv";
		exit(-1);
	}
	char input[256];
	while (!definitionFile.eof())
	{
		definitionFile.getline(input, 255);
		string inputStr(input);
		int dbgprovNum = atoi(inputStr.substr(0, inputStr.find_first_of(';')).c_str());
		if (
			(inputStr.substr(0, 8) == "province") ||
			(inputStr.substr(inputStr.find_last_of(';') + 1, 6) == "Unused") ||
			(inputStr.substr(inputStr.find_last_of(';') + 1, 3) == "RNW")
			)
		{
			continue;
		}
		int provNum = atoi(inputStr.substr(0, inputStr.find_first_of(';')).c_str());
		validProvinces.insert(make_pair(provNum, provNum));
	}

	return validProvinces;
}


void EU4::world::loadCountries(istream& theStream)
{
	countries processedCountries(theStream);
	auto theProcessedCountries = processedCountries.getTheCountries();
	theCountries.swap(theProcessedCountries);
}


void EU4::world::loadRevolutionTargetString(const shared_ptr<Object> EU4SaveObj)
{
	vector<shared_ptr<Object>> revolutionTargetObj = EU4SaveObj->getValue("revolution_target");
	if (revolutionTargetObj.size() > 0)
	{
		revolutionTargetString = revolutionTargetObj[0]->getLeaf();
	}
}


void EU4::world::loadRevolutionTarget()
{
	if (revolutionTargetString != "")
	{
		auto country = theCountries.find(revolutionTargetString);
		if (country != theCountries.end())
		{
			country->second->viveLaRevolution(true);
		}
	}
}


void EU4::world::addProvinceInfoToCountries()
{
	// add province owner info to countries
	for (map<int, EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		auto j = theCountries.find( i->second->getOwnerString() );
		if (j != theCountries.end())
		{
			j->second->addProvince(i->second);
			i->second->setOwner(j->second);
		}
	}

	// add province core info to countries
	for (auto province: provinces)
	{
		auto cores = province.second->getCores(theCountries);	// the cores held on this province
		for (auto core: cores)
		{
			core->addCore(province.second);
		}
	}
}


void EU4::world::loadDiplomacy(const shared_ptr<Object> EU4SaveObj)
{
	vector<shared_ptr<Object>> diploObj = EU4SaveObj->getValue("diplomacy");	// the object holding the world's diplomacy
	if (diploObj.size() > 0)
	{
		diplomacy = new EU4Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = new EU4Diplomacy;
	}
}


void EU4::world::determineProvinceWeights()
{
	// calculate total province weights
	worldWeightSum = 0;

	/*ofstream EU4_Production("EU4_Production.csv");
	ofstream EU4_Tax("EU4_TaxIncome.csv");
	ofstream EU4_World("EU4_World.csv");*/

	std::vector<double> provEconVec;

	std::map<string, vector<double> > world_tag_weights;

	//// Heading
	//EU4_Production << "PROV NAME" << ",";
	//EU4_Production << "OWNER" << ",";
	//EU4_Production << "TRADE GOOD" << ",";
	//EU4_Production << "GOODS PROD" << ",";
	//EU4_Production << "PRICE" << ",";
	//EU4_Production << "TRADE EFF" << ",";
	//EU4_Production << "PROD EFF" << ",";
	//EU4_Production << "PROV TRADE VAL" << ",";
	//EU4_Production << "TOTAL TRADE VAL" << ",";
	//EU4_Production << "TOTAL PRODUCTION" << endl;

	//// Heading
	//EU4_World << "COUNTRY" << ",";
	//EU4_World << "BASE TAX (2x)" << ",";
	//EU4_World << "TAX INCOME" << ",";
	//EU4_World << "PRODUCTION" << ",";
	//EU4_World << "BUILDINGS" << ",";
	//EU4_World << "MANPOWER" << ",";
	//EU4_World << "SUBTOTAL SAN BUILD" << ",";
	//EU4_World << "TOTAL WEIGHT" << endl;

	//// Heading
	//EU4_Tax << "PROV NAME" << ",";
	//EU4_Tax << "OWNER" << ",";
	//EU4_Tax << "BASE TAX" << ",";
	//EU4_Tax << "BUILD INCOME" << ",";
	//EU4_Tax << "TAX EFF" << ",";
	//EU4_Tax << "TOTAL TAX INCOME" << endl;
	for (map<int, EU4Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		i->second->determineProvinceWeight();
		// 0: Goods produced; 1 trade goods price; 2: trade value efficiency; 3: production effiency; 4: trade value; 5: production income
		// 6: base tax; 7: building tax income 8: building tax eff; 9: total tax income; 10: total_trade_value


		provEconVec = i->second->getProvProductionVec();
		/*EU4_Production << i->second->getProvName() << ",";
		EU4_Production << i->second->getOwnerString() << ",";
		EU4_Production << i->second->getTradeGoods() << ",";
		EU4_Production << provEconVec.at(0) << ",";
		EU4_Production << provEconVec.at(1) << ",";
		EU4_Production << provEconVec.at(2) << ",";
		EU4_Production << provEconVec.at(3) << ",";
		EU4_Production << provEconVec.at(4) << ",";
		EU4_Production << provEconVec.at(10) << ",";
		EU4_Production << i->second->getProvProdIncome() << "," << endl;


		EU4_Tax << i->second->getProvName() << ",";
		EU4_Tax << i->second->getOwnerString() << ",";
		EU4_Tax << provEconVec.at(6) << ",";
		EU4_Tax << provEconVec.at(7) << ",";
		EU4_Tax << provEconVec.at(8) << ",";
		EU4_Tax << provEconVec.at(9) << "," << endl;*/

		worldWeightSum += i->second->getTotalWeight();

		vector<double> map_values;
		// Total Base Tax, Total Tax Income, Total Production, Total Buildings, Total Manpower, total province weight //
		map_values.push_back((2 * i->second->getBaseTax()));
		map_values.push_back(i->second->getProvTaxIncome());
		map_values.push_back(i->second->getProvProdIncome());
		map_values.push_back(i->second->getProvTotalBuildingWeight());
		map_values.push_back(i->second->getProvMPWeight());
		map_values.push_back(i->second->getTotalWeight());

		if (world_tag_weights.count(i->second->getOwnerString())) {
			vector<double> new_map_values;
			new_map_values = world_tag_weights[i->second->getOwnerString()];
			new_map_values[0] += map_values[0];
			new_map_values[1] += map_values[1];
			new_map_values[2] += map_values[2];
			new_map_values[3] += map_values[3];
			new_map_values[4] += map_values[4];
			new_map_values[5] += map_values[5];

			world_tag_weights[i->second->getOwnerString()] = new_map_values;

		}
		else {
			world_tag_weights.insert(std::pair<string, vector<double> >(i->second->getOwnerString(), map_values));
		}

	}
	LOG(LogLevel::Info) << "Sum of all Province Weights: " << worldWeightSum;

	// Total Base Tax, Total Tax Income, Total Production, Total Buildings, Total Manpower, total province weight //
	LOG(LogLevel::Info) << "World Tag Map Size: " << world_tag_weights.size();

	/*for (map<string, vector<double> >::iterator i = world_tag_weights.begin(); i != world_tag_weights.end(); i++)
	{
	EU4_World << i->first << ",";
	EU4_World << i->second[0] << ",";
	EU4_World << i->second[1] << ",";
	EU4_World << i->second[2] << ",";
	EU4_World << i->second[3] << ",";
	EU4_World << i->second[4] << ",";
	EU4_World << (i->second[5] - i->second[3]) << ",";
	EU4_World << i->second[5] << endl;
	}

	EU4_Production.close();
	EU4_Tax.close();
	EU4_World.close();*/
}


void EU4::world::checkAllEU4CulturesMapped() const
{
	for (auto cultureItr: EU4::cultureGroups::getCultureToGroupMap())
	{
		string Vi2Culture;

		string	EU4Culture	= cultureItr.first;
		bool		matched		= mappers::cultureMapper::cultureMatch(EU4Culture, Vi2Culture);
		if (!matched)
		{
			LOG(LogLevel::Warning) << "No culture mapping for EU4 culture " << EU4Culture;
		}
	}
}


void EU4::world::readCommonCountries()
{
	LOG(LogLevel::Info) << "Reading EU4 common/countries";
	ifstream commonCountries(Configuration::getEU4Path() + "/common/country_tags/00_countries.txt");	// the data in the countries file
	readCommonCountriesFile(commonCountries, Configuration::getEU4Path());
	for (auto itr: Configuration::getEU4Mods())
	{
		set<string> fileNames;
		Utils::GetAllFilesInFolder(itr + "/common/country_tags/", fileNames);
		for (set<string>::iterator fileItr = fileNames.begin(); fileItr != fileNames.end(); fileItr++)
		{
			ifstream convertedCommonCountries(itr + "/common/country_tags/" + *fileItr);	// a stream of the data in the converted countries file
			readCommonCountriesFile(convertedCommonCountries, itr);
		}
	}
}


void EU4::world::readCommonCountriesFile(istream& in, const std::string& rootPath)
{
	// Add any info from common\countries
	const int maxLineLength = 10000;	// the maximum line length
	char line[maxLineLength];			// the line being processed

	while (true)
	{
		in.getline(line, maxLineLength);
		if (in.eof())
		{
			return;
		}
		std::string countryLine = line;
		if (countryLine.size() >= 6 && countryLine[0] != '#')
		{
			// First three characters must be the tag.
			std::string tag = countryLine.substr(0, 3);
			auto findIter = theCountries.find(tag);
			if (findIter != theCountries.end())
			{
				auto country = findIter->second;

				// The country file name is all the text after the equals sign (possibly in quotes).
				size_t commentPos	= countryLine.find('#', 3);
				if (commentPos != string::npos)
				{
					countryLine = countryLine.substr(0, commentPos);
				}
				size_t equalPos	= countryLine.find('=', 3);
				size_t beginPos	= countryLine.find_first_not_of(' ', equalPos + 1);
				size_t endPos		= countryLine.find_last_of('\"') + 1;
				std::string fileName = countryLine.substr(beginPos, endPos - beginPos);
				if (fileName.front() == '"' && fileName.back() == '"')
				{
					fileName = fileName.substr(1, fileName.size() - 2);
				}
				std::replace(fileName.begin(), fileName.end(), '/', '/');

				// Parse the country file.
				std::string fullFilename = rootPath + "/common/" + fileName;
				size_t lastPathSeparatorPos = fullFilename.find_last_of('/');
				std::string localFileName = fullFilename.substr(lastPathSeparatorPos + 1, string::npos);
				country->readFromCommonCountry(localFileName, fullFilename);
			}
		}
	}
}


void EU4::world::setLocalisations()
{
	LOG(LogLevel::Info) << "Reading localisation";
	EU4Localisation localisation;
	localisation.ReadFromAllFilesInFolder(Configuration::getEU4Path() + "/localisation");
	for (auto itr: Configuration::getEU4Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod localisation";
		localisation.ReadFromAllFilesInFolder(itr + "/localisation");
	}

	for (auto theCountry: theCountries)
	{
		const auto& nameLocalisations = localisation.GetTextInEachLanguage(theCountry.second->getTag());	// the names in all languages
		for (const auto& nameLocalisation : nameLocalisations)	// the name under consideration
		{
			const std::string& language = nameLocalisation.first;	// the language
			const std::string& name = nameLocalisation.second;		// the name of the country in this language
			theCountry.second->setLocalisationName(language, name);
		}
		const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(theCountry.second->getTag() + "_ADJ");	// the adjectives in all languages
		for (const auto& adjectiveLocalisation : adjectiveLocalisations)	// the adjective under consideration
		{
			const std::string& language = adjectiveLocalisation.first;		// the language
			const std::string& adjective = adjectiveLocalisation.second;	// the adjective for the country in this language
			theCountry.second->setLocalisationAdjective(language, adjective);
		}
	}
}


void EU4::world::resolveRegimentTypes()
{
	LOG(LogLevel::Info) << "Resolving unit types.";
	RegimentTypeMap rtm;
	fstream read;
	read.open("unit_strength.txt");
	if (read.is_open())
	{
		read.close();
		read.clear();
		LOG(LogLevel::Info) << "\tReading unit strengths from unit_strength.txt";
		shared_ptr<Object> unitsObj = parser_UTF8::doParseFile("unit_strength.txt");
		if (unitsObj == NULL)
		{
			LOG(LogLevel::Error) << "Could not parse file unit_strength.txt";
			exit(-1);
		}
		for (int i = 0; i < num_reg_categories; ++i)
		{
			AddCategoryToRegimentTypeMap(unitsObj, (RegimentCategory)i, RegimentCategoryNames[i], rtm);
		}
	}
	else
	{
		LOG(LogLevel::Info) << "\tReading unit strengths from EU4 installation folder";

		set<string> filenames;
		Utils::GetAllFilesInFolder(Configuration::getEU4Path() + "/common/units/", filenames);
		for (auto filename: filenames)
		{
			AddUnitFileToRegimentTypeMap((Configuration::getEU4Path() + "/common/units"), filename, rtm);
		}
	}
	read.close();
	read.clear();

	for (auto itr = theCountries.begin(); itr != theCountries.end(); ++itr)
	{
		itr->second->resolveRegimentTypes(rtm);
	}
}


// todo: move the getting of rules into its own mapper, with a merge rule structure type
//		then break out things into subfunctions with a give rule as a parameter
void EU4::world::mergeNations()
{
	LOG(LogLevel::Info) << "Merging nations";
	shared_ptr<Object> mergeObj = parser_UTF8::doParseFile("merge_nations.txt");
	if (mergeObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file merge_nations.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> rules = mergeObj->getValue("merge_nations");
	if (rules.size() < 0)
	{
		LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
		return;
	}

	rules = rules[0]->getLeaves();
	for (auto rule: rules)
	{
		if ((rule->getKey() == "merge_daimyos") && (rule->getLeaf() == "yes"))
		{
			uniteJapan();
			continue;
		}

		vector<shared_ptr<Object>> ruleItems = rule->getLeaves();

		string masterTag;
		vector<string> slaveTags;
		bool enabled = false;
		for (auto item: ruleItems)
		{
			if ((item->getKey() == "merge") && (item->getLeaf() == "yes"))
			{
				enabled = true;
			}
			else if (item->getKey() == "master")
			{
				masterTag = item->getLeaf();
			}
			else if (item->getKey() == "slave")
			{
				slaveTags.push_back(item->getLeaf());
			}
		}

		auto master = getCountry(masterTag);
		if (enabled && master)
		{
			for (auto slaveTag: slaveTags)
			{
				auto slave = getCountry(slaveTag);
				if (slave)
				{
					master->eatCountry(slave, master);
				}
			}
		}
	}
}


void EU4::world::uniteJapan()
{
	shared_ptr<EU4::Country> japan;

	auto version20 = EU4Version("1.20.0.0");
	if (*version >= version20)
	{
		for (auto country : theCountries)
		{
			if (country.second->getPossibleShogun())
			{
				string tag = country.first;
				japan = getCountry(tag);
			}
		}
	}
	else 
	{
		japan = getCountry("JAP");
	}
	if (japan == nullptr)
	{
		return;
	}
	if (japan->hasFlag("united_daimyos_of_japan"))
	{
		return;
	}

	for (auto country: theCountries)
	{
		if (country.second->getPossibleDaimyo())
		{
			japan->eatCountry(country.second, japan);
		}
	}
}


void EU4::world::checkAllProvincesMapped() const
{
	for (auto province: provinces)
	{
		auto Vic2Provinces = provinceMapper::getVic2ProvinceNumbers(province.first);
		if (Vic2Provinces.size() == 0)
		{
			LOG(LogLevel::Warning) << "No mapping for province " << province.first;
		}
	}
}


void EU4::world::setNumbersOfDestinationProvinces()
{
	for (auto province: provinces)
	{
		auto Vic2Provinces = provinceMapper::getVic2ProvinceNumbers(province.first);
		province.second->setNumDestV2Provs(Vic2Provinces.size());
	}
}


void EU4::world::checkAllEU4ReligionsMapped() const
{
	for (auto EU4Religion: EU4Religion::getAllReligions())
	{
		auto Vic2Religion = religionMapper::getVic2Religion(EU4Religion.first);
		if (Vic2Religion == "")
		{
			Log(LogLevel::Warning) << "No religion mapping for EU4 religion " << EU4Religion.first;
		}
	}
}


void EU4::world::removeEmptyNations()
{
	std::map<std::string, std::shared_ptr<EU4::Country>> survivingCountries;

	for (auto country: theCountries)
	{
		vector<EU4Province*> provinces = country.second->getProvinces();
		vector<EU4Province*> cores = country.second->getCores();
		if ((provinces.size() == 0) && (cores.size() == 0))
		{
			LOG(LogLevel::Debug) << "Removing empty nation " << country.first;
		}
		else
		{
			survivingCountries.insert(country);
		}
	}

	theCountries.swap(survivingCountries);
}


void EU4::world::removeDeadLandlessNations()
{
	std::map<std::string, std::shared_ptr<EU4::Country>> landlessCountries;
	for (auto country: theCountries)
	{
		vector<EU4Province*> provinces = country.second->getProvinces();
		if (provinces.size() == 0)
		{
			landlessCountries.insert(country);
		}
	}

	for (auto country: landlessCountries)
	{
		if (!country.second->cultureSurvivesInCores())
		{
			theCountries.erase(country.first);
			LOG(LogLevel::Debug) << "Removing dead landless nation " << country.first;
		}
	}
}


void EU4::world::removeLandlessNations()
{
	std::map<std::string, std::shared_ptr<EU4::Country>> survivingCountries;

	for (auto country: theCountries)
	{
		auto provinces = country.second->getProvinces();
		if (provinces.size() == 0)
		{
			LOG(LogLevel::Debug) << "Removing landless nation " << country.first;
		}
		else
		{
			survivingCountries.insert(country);
		}
	}

	theCountries.swap(survivingCountries);
}


void EU4::world::setEmpires()
{
	for (auto country: theCountries)
	{
		// set HRE stuff
		auto capitalItr = provinces.find(country.second->getCapital());
		if ((capitalItr != provinces.end()) && (capitalItr->second->getInHRE()))
		{
			country.second->setInHRE(true);
		}
		if (country.second->getTag() == holyRomanEmperor)
		{
			country.second->setEmperor(true);
		}
		if (country.second->getTag() == celestialEmperor)
		{
			country.second->setCelestialEmperor(true);
		}
	}
}


std::shared_ptr<EU4::Country> EU4::world::getCountry(std::string tag) const
{
	auto i = theCountries.find(tag);
	if (i != theCountries.end())
	{
		return i->second;
	}
	else
	{
		return {};
	}
}


EU4Province* EU4::world::getProvince(const int provNum) const
{
	map<int, EU4Province*>::const_iterator i = provinces.find(provNum);
	return (i != provinces.end()) ? i->second : NULL;
}


bool EU4::world::isRandomWorld() const
{
	bool isRandomWorld = true;

	for (auto sourceCountry: theCountries)
	{
		if (sourceCountry.first[0] != 'D' && sourceCountry.second->getRandomName().empty())
		{
			isRandomWorld = false;
		}
	}

	return isRandomWorld;
}