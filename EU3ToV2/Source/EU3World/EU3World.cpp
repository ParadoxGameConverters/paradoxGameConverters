/*Copyright (c) 2014 The Paradox Game Converters Project

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


#include "EU3World.h"
#include <algorithm>
#include <fstream>
#include "../Log.h"
#include "../Configuration.h"
#include "../Mapper.h"
#include "../Parsers/Object.h"
#include "../Parsers/Parser.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Diplomacy.h"
#include "EU3Localisation.h"
#include "EU3Religion.h"



EU3World::EU3World(Object* obj)
{
	cachedWorldType = unknown;

	string key;	
	vector<Object*> leaves = obj->getLeaves();

	provinces.clear();
	countries.clear();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		key = leaves[i]->getKey();

		// Is this a numeric value? If so, must be a province
		if (atoi(key.c_str()) > 0)
		{
			EU3Province* province = new EU3Province(leaves[i]);
			provinces.insert(make_pair(province->getNum(), province));
		}

		// Countries are three uppercase characters
		else if ((key.size() == 3) && 
					(key.c_str()[0] >= 'A') && (key.c_str()[0] <= 'Z') && 
					(key.c_str()[1] >= 'A') && (key.c_str()[1] <= 'Z') && 
					(key.c_str()[2] >= 'A') && (key.c_str()[2] <= 'Z')
				  )
		{
			if ((key == "---") || (key == "REB") || (key == "PIR") || (key == "NAT"))
			{
				continue;
			}
			else
			{
				EU3Country* country = new EU3Country(leaves[i]);
				countries.insert(make_pair(country->getTag(), country));
			}
		}
	}

	// add province owner info to countries
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		map<string, EU3Country*>::iterator j = countries.find( i->second->getOwnerString() );
		if (j != countries.end())
		{
			j->second->addProvince(i->second);
			i->second->setOwner(j->second);
		}
	}

	// add province core info to countries
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		vector<EU3Country*> cores = i->second->getCores(countries);
		for (vector<EU3Country*>::iterator j = cores.begin(); j != cores.end(); j++)
		{
			(*j)->addCore(i->second);
		}
	}

	vector<Object*> diploObj = obj->getValue("diplomacy");
	if (diploObj.size() > 0)
	{
		diplomacy = new EU3Diplomacy(diploObj[0]);
	}
	else
	{
		diplomacy = new EU3Diplomacy;
	}

	vector<Object*> tradeObj = obj->getValue("trade");
	if (tradeObj.size() > 0)
	{
		vector<Object*> COTsObj = tradeObj[0]->getValue("cot");
		for (vector<Object*>::iterator i = COTsObj.begin(); i != COTsObj.end(); i++)
		{
			int location = atoi( (*i)->getValue("location")[0]->getLeaf().c_str() );
			map<int, EU3Province*>::iterator j = provinces.find(location);
			if ( j != provinces.end() )
			{
				j->second->setCOT(true);
			}
		}
	}

	// calculate total province weights
	worldWeightSum = 0;

	ofstream EU4_Production("EU4_Production.csv");
	ofstream EU4_Tax("EU4_TaxIncome.csv");
	ofstream EU4_World("EU4_World.csv");

	std::vector<double> provEconVec;

	std::map<string, vector<double> > world_tag_weights;

	// Heading
	EU4_Production << "PROV NAME" << ",";
	EU4_Production << "OWNER" << ",";
	EU4_Production << "TRADE GOOD" << ",";
	EU4_Production << "GOODS PROD" << ",";
	EU4_Production << "PRICE" << ",";
	EU4_Production << "TRADE EFF" << ",";
	EU4_Production << "PROD EFF" << ",";
	EU4_Production << "PROV TRADE VAL" << ",";
	EU4_Production << "TOTAL TRADE VAL" << ",";
	EU4_Production << "TOTAL PRODUCTION" << endl;

	// Heading
	EU4_World << "COUNTRY" << ",";
	EU4_World << "BASE TAX (2x)" << ",";
	EU4_World << "TAX INCOME" << ",";
	EU4_World << "PRODUCTION" << ",";
	EU4_World << "BUILDINGS" << ",";
	EU4_World << "MANPOWER" << ",";
	EU4_World << "SUBTOTAL SAN BUILD" << ",";
	EU4_World << "TOTAL WEIGHT" << endl;

	// Heading
	EU4_Tax << "PROV NAME" << ",";
	EU4_Tax << "OWNER" << ",";
	EU4_Tax << "BASE TAX" << ",";
	EU4_Tax << "BUILD INCOME" << ",";
	EU4_Tax << "TAX EFF" << ",";
	EU4_Tax << "TOTAL TAX INCOME" << endl;
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		i->second->determineProvinceWeight();
		// 0: Goods produced; 1 trade goods price; 2: trade value efficiency; 3: production effiency; 4: trade value; 5: production income
		// 6: base tax; 7: building tax income 8: building tax eff; 9: total tax income; 10: total_trade_value


		provEconVec = i->second->getProvProductionVec();
		EU4_Production << i->second->getProvName() << ",";
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
		EU4_Tax << provEconVec.at(9) << "," << endl;

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

	for (map<string, vector<double> >::iterator i = world_tag_weights.begin(); i != world_tag_weights.end(); i++)
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
	EU4_World.close();
}


void EU3World::setEU3WorldProvinceMappings(const inverseProvinceMapping& inverseProvinceMap)
{
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		i->second->setNumDestV2Provs(inverseProvinceMap.find(i->first)->second.size());
	}
}


void EU3World::readCommonCountries(istream& in, const std::string& rootPath)
{
	// Add any info from common\countries
	const int maxLineLength = 10000;
	char line[maxLineLength];

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
			map<string, EU3Country*>::iterator findIter = countries.find(tag);
			if (findIter != countries.end())
			{
				EU3Country* country = findIter->second;

				// The country file name is all the text after the equals sign (possibly in quotes).
				size_t equalPos = countryLine.find('=', 3);
				size_t beginPos = countryLine.find_first_not_of(' ', equalPos + 1);
				size_t endPos = countryLine.find_last_not_of(' ') + 1;
				std::string fileName = countryLine.substr(beginPos, endPos - beginPos);
				if (fileName.front() == '"' && fileName.back() == '"')
				{
					fileName = fileName.substr(1, fileName.size() - 2);
				}
				std::replace(fileName.begin(), fileName.end(), '/', '\\');

				// Parse the country file.
				std::string path = rootPath + "\\common\\" + fileName;
				size_t lastPathSeparatorPos = path.find_last_of('\\');
				std::string localFileName = path.substr(lastPathSeparatorPos + 1, string::npos);
				country->readFromCommonCountry(localFileName, doParseFile(path.c_str()));
			}
		}
	}
}


EU3Country* EU3World::getCountry(string tag) const
{
	map<string, EU3Country*>::const_iterator i = countries.find(tag);
	if (i != countries.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


EU3Province* EU3World::getProvince(int provNum) const
{
	map<int, EU3Province*>::const_iterator i = provinces.find(provNum);
	if (i != provinces.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}


void EU3World::removeCountry(string tag)
{
	countries.erase(tag);
}


void EU3World::resolveRegimentTypes(const RegimentTypeMap& rtMap)
{
	for (map<string, EU3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		itr->second->resolveRegimentTypes(rtMap);
	}
}


WorldType EU3World::getWorldType()
{
	if (cachedWorldType != unknown)
	{
		return cachedWorldType;
	}

	int maxProvinceID = 0;
	for (map<int, EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); ++itr)
	{
		if ( itr->first > maxProvinceID )
		{
			maxProvinceID = itr->first;
		}
	}

	switch (maxProvinceID)
	{
	case 1774:
	case 1775:
		cachedWorldType = InNomine;
		break;
	case 1814:
		cachedWorldType = HeirToTheThrone;
		break;
	case 1882:
		cachedWorldType = DivineWind;
		break;
	default:
		Log(LogLevel::Warning) << "Unrecognized max province ID: " << maxProvinceID;
		if (maxProvinceID < 1774)
		{
			cachedWorldType = VeryOld; // pre-IN
		}
		else
		{
			cachedWorldType = unknown; // possibly modded
		}
		break;
	}

	// Allow the configuration file to override the game type
	string configWorldType = Configuration::getEU3Gametype();
	WorldType forcedWorldType = unknown;
	if (configWorldType == "dw")
	{
		forcedWorldType = DivineWind;
	}
	else if (configWorldType == "httt")
	{
		forcedWorldType = HeirToTheThrone;
	}
	else if (configWorldType == "in")
	{
		forcedWorldType = InNomine;
	}
	else if (configWorldType == "auto")
	{
		forcedWorldType = cachedWorldType;
	}

	if ((cachedWorldType != forcedWorldType) && (cachedWorldType != unknown))
	{
		Log(LogLevel::Warning) << "World type was detected successfuly, but a different type was specified in the configuration file!";
	}

	if (cachedWorldType == unknown)
	{
		Log(LogLevel::Warning) << "World type unknown!";
	}

	if (forcedWorldType != unknown)
	{
		cachedWorldType = forcedWorldType;
	}

	return cachedWorldType;
}


void EU3World::checkAllProvincesMapped(const inverseProvinceMapping& inverseProvinceMap) const
{
	for (map<int, EU3Province*>::const_iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		inverseProvinceMapping::const_iterator j = inverseProvinceMap.find(i->first);
		if (j == inverseProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for province " << i->first;
		}
	}
}


void EU3World::checkAllEU3CulturesMapped(const cultureMapping& cultureMap, const inverseUnionCulturesMap& inverseUnionCultures) const
{
	for (auto cultureItr = inverseUnionCultures.begin(); cultureItr != inverseUnionCultures.end(); cultureItr++)
	{
		string	EU3Culture	= cultureItr->first;
		bool		matched		= false;
		for (auto mapItr = cultureMap.begin(); mapItr != cultureMap.end(); mapItr++)
		{
			if (mapItr->srcCulture == EU3Culture)
			{
				matched = true;
				break;
			}
		}
		if (!matched)
		{
			LOG(LogLevel::Warning) << "No culture mapping for EU3 culture " << EU3Culture;
		}
	}
}


void EU3World::checkAllEU3ReligionsMapped(const religionMapping& religionMap) const
{
	map<string, EU3Religion*> allReligions = EU3Religion::getAllReligions();
	for (auto religionItr = allReligions.begin(); religionItr != allReligions.end(); ++religionItr)
	{
		auto mapItr = religionMap.find(religionItr->first);
		if (mapItr == religionMap.end())
		{
			Log(LogLevel::Warning) << "No religion mapping for EU3 religion " << religionItr->first;
		}
	}
}


void EU3World::setLocalisations(EU3Localisation& localisation)
{
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		const auto& nameLocalisations = localisation.GetTextInEachLanguage(countryItr->first);
		for (const auto& nameLocalisation : nameLocalisations)
		{
			const std::string& language = nameLocalisation.first;
			const std::string& name = nameLocalisation.second;
			countryItr->second->setLocalisationName(language, name);
		}
		const auto& adjectiveLocalisations = localisation.GetTextInEachLanguage(countryItr->first + "_ADJ");
		for (const auto& adjectiveLocalisation : adjectiveLocalisations)
		{
			const std::string& language = adjectiveLocalisation.first;
			const std::string& adjective = adjectiveLocalisation.second;
			countryItr->second->setLocalisationAdjective(language, adjective);
		}
	}
}