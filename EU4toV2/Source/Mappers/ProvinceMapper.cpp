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
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include "../Configuration.h"
#include "../EU4World/EU4Version.h"



provinceMapper* provinceMapper::instance = NULL;



provinceMapper::provinceMapper()
{
	LOG(LogLevel::Info) << "Parsing province mappings";
	shared_ptr<Object> provinceMappingObj = parser_UTF8::doParseFile("province_mappings.txt");
	if (provinceMappingObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file province_mappings.txt";
		exit(-1);
	}

	initProvinceMap(provinceMappingObj);
}


void provinceMapper::initProvinceMap(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> versions = obj->getLeaves();
	if (versions.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}

	unsigned int mappingsIdx = getMappingsIndex(versions);
	auto mappings = versions[mappingsIdx]->getLeaves();
	for (auto mapping: mappings)
	{
		createMappings(mapping);
	}
}


int provinceMapper::getMappingsIndex(vector<shared_ptr<Object>> versions)
{
	unsigned int mappingsIdx;

	auto saveVersion = Configuration::getEU4Version();
	for (mappingsIdx = 0; mappingsIdx < versions.size(); mappingsIdx++)
	{
		auto mappingVersion = EU4Version(versions[mappingsIdx]->getKey());
		if (saveVersion >= mappingVersion)
		{
			break;
		}
	}
	LOG(LogLevel::Debug) << "Using version " << versions[mappingsIdx]->getKey() << " mappings";

	return mappingsIdx;
}


void provinceMapper::createMappings(shared_ptr<Object> mapping)
{
	vector<int> EU4Numbers;
	vector<int> Vic2Numbers;
	bool resettable = false;

	for (auto mappingItem: mapping->getLeaves())
	{
		if (mappingItem->getKey() == "eu4")
		{
			EU4Numbers.push_back(stoi(mappingItem->getLeaf()));
		}
		else if (mappingItem->getKey() == "v2")
		{
			Vic2Numbers.push_back(stoi(mappingItem->getLeaf()));
		}
		else if (mappingItem->getKey() == "resettable")
		{
			resettable = true;
		}
		else
		{
			LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
		}
	}

	if (EU4Numbers.size() == 0)
	{
		EU4Numbers.push_back(0);
	}
	if (Vic2Numbers.size() == 0)
	{
		Vic2Numbers.push_back(0);
	}

	for (auto Vic2Number: Vic2Numbers)
	{
		if (Vic2Number != 0)
		{
			Vic2ToEU4ProvinceMap.insert(make_pair(Vic2Number, EU4Numbers));
			if (resettable)
			{
				resettableProvinces.insert(Vic2Number);
			}
		}
	}
	for (auto EU4Number: EU4Numbers)
	{
		if (EU4Number != 0)
		{
			EU4ToVic2ProvinceMap.insert(make_pair(EU4Number, Vic2Numbers));
		}
	}
}


const vector<int> provinceMapper::GetVic2ProvinceNumbers(const int EU4ProvinceNumber)
{
	auto mapping = EU4ToVic2ProvinceMap.find(EU4ProvinceNumber);
	if (mapping != EU4ToVic2ProvinceMap.end())
	{
		return mapping->second;
	}
	else
	{
		vector<int> empty;
		return empty;
	}
}


const vector<int> provinceMapper::GetEU4ProvinceNumbers(int Vic2ProvinceNumber)
{
	auto mapping = Vic2ToEU4ProvinceMap.find(Vic2ProvinceNumber);
	if (mapping != Vic2ToEU4ProvinceMap.end())
	{
		return mapping->second;
	}
	else
	{
		vector<int> empty;
		return empty;
	}
}


bool provinceMapper::IsProvinceResettable(int Vic2ProvinceNumber)
{
	if (resettableProvinces.count(Vic2ProvinceNumber) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}