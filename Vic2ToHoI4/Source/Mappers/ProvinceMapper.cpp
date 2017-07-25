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



#include "ProvinceMapper.h"
#include <fstream>
#include "Log.h"
#include "../Configuration.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"




provinceMapper* provinceMapper::instance = NULL;



provinceMapper::provinceMapper()
{
	LOG(LogLevel::Info) << "Parsing province mappings";
	shared_ptr<Object> parsedMappingsFile = parser_8859_15::doParseFile("province_mappings.txt");
	if (parsedMappingsFile == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file province_mappings.txt";
		exit(-1);
	}

	initProvinceMap(parsedMappingsFile);
}



void provinceMapper::initProvinceMap(shared_ptr<Object> parsedMappingsFile)
{
	vector<shared_ptr<Object>> versions = parsedMappingsFile->getLeaves();
	if (versions.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		exit(-1);
	}

	vector<shared_ptr<Object>> mappings = getCorrectMappingVersion(versions);
	processMappings(mappings);
	checkAllHoI4ProvinesMapped();
}


void provinceMapper::processMappings(const vector<shared_ptr<Object>>& mappings)
{
	for (auto mapping: mappings)
	{
		vector<int> Vic2Nums;
		vector<int> HoI4Nums;

		for (auto item: mapping->getLeaves())
		{
			if (item->getKey() == "vic2")
			{
				Vic2Nums.push_back(stoi(item->getLeaf()));
			}
			else if (item->getKey() == "hoi4")
			{
				HoI4Nums.push_back(stoi(item->getLeaf()));
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
			}
		}

		if (Vic2Nums.size() == 0)
		{
			Vic2Nums.push_back(0);
		}
		if (HoI4Nums.size() == 0)
		{
			HoI4Nums.push_back(0);
		}

		insertIntoHoI4ToVic2ProvinceMap(Vic2Nums, HoI4Nums);
		insertIntoVic2ToHoI4ProvinceMap(Vic2Nums, HoI4Nums);
	}
}


void provinceMapper::insertIntoHoI4ToVic2ProvinceMap(const vector<int>& Vic2Nums, const vector<int>& HoI4Nums)
{
	for (auto num: HoI4Nums)
	{
		if (num != 0)
		{
			HoI4ToVic2ProvinceMap.insert(make_pair(num, Vic2Nums));
		}
	}
}


void provinceMapper::insertIntoVic2ToHoI4ProvinceMap(const vector<int>& Vic2Nums, const vector<int>& HoI4Nums)
{
	for (auto num: Vic2Nums)
	{
		if (num != 0)
		{
			Vic2ToHoI4ProvinceMap.insert(make_pair(num, HoI4Nums));
		}
	}
}


void provinceMapper::checkAllHoI4ProvinesMapped()
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		int provNum = getNextProvinceNumFromFile(definitions);
		if (provNum == -1)
		{
			break;
		}

		verifyProvinceIsMapped(provNum);
	}

	definitions.close();
}


int provinceMapper::getNextProvinceNumFromFile(ifstream& definitions)
{
	string line;
	getline(definitions, line);
	int pos = line.find_first_of(';');
	if (pos != string::npos)
	{
		return stoi(line.substr(0, pos));
	}
	else
	{
		return -1;
	}
}


void provinceMapper::verifyProvinceIsMapped(int provNum)
{
	if (provNum != 0)
	{
		auto num = HoI4ToVic2ProvinceMap.find(provNum);
		if (num == HoI4ToVic2ProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI4 province " << provNum;
		}
	}
}


vector<shared_ptr<Object>> provinceMapper::getCorrectMappingVersion(const vector<shared_ptr<Object>>& versions)
{
	for (auto version: versions)
	{
		HOI4Version currentVersion(version->getKey());
		if (Configuration::getHOI4Version() >= currentVersion)
		{
			LOG(LogLevel::Debug) << "Using version " << version->getKey() << " mappings";
			return version->getLeaves();
		}
	}

	LOG(LogLevel::Debug) << "Using version " << versions[versions.size() - 1]->getKey() << " mappings";
	return versions[versions.size() - 1]->getLeaves();
}


vector<int> provinceMapper::getHoI4ProvinceNums(const int v2ProvinceNum)
{
	static const vector<int> empty_vec;	// an empty vector in case there are no equivalent V2 province numbers

	auto itr = Vic2ToHoI4ProvinceMap.find(v2ProvinceNum);
	if (itr == Vic2ToHoI4ProvinceMap.end())
	{
		return empty_vec;
	}
	else
	{
		return itr->second;
	}
}