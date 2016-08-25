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



#include "ProvinceMapper.h"
#include <fstream>
#include "log.h"
#include "..\Configuration.h"
#include "Object.h"
#include "ParadoxParser8859_15.h"




provinceMapper* provinceMapper::instance = NULL;



provinceMapper::provinceMapper()
{
	LOG(LogLevel::Info) << "Parsing province mappings";
	Object* obj = parser_8859_15::doParseFile("province_mappings.txt");
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file province_mappings.txt";
		exit(-1);
	}

	initProvinceMap(obj);
}



void provinceMapper::initProvinceMap(Object* obj)
{
	vector<Object*> leaves = obj->getLeaves();	// the different version number blocks

	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}

	vector<Object*> data = leaves[0]->getLeaves();	// the actual mappings
	for (auto mapping: data)
	{
		vector<int> V2nums;					// the V2 province numbers in this mappping
		vector<int> HoI4nums;				// the HoI4 province numbers in this mappping
		bool			resettable = false;	// if this is a province that can be reset to V2 defaults

		vector<Object*> vMaps = mapping->getLeaves();	// the items within the mapping
		for (auto item: vMaps)
		{
			if (item->getKey() == "vic2")
			{
				V2nums.push_back(atoi(item->getLeaf().c_str()));
			}
			else if (item->getKey() == "hoi4")
			{
				HoI4nums.push_back(atoi(item->getLeaf().c_str()));
			}
			else if (item->getKey() == "resettable")
			{
				resettable = true;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
			}
		}

		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}
		if (HoI4nums.size() == 0)
		{
			HoI4nums.push_back(0);
		}

		for (auto num: HoI4nums)
		{
			if (num != 0)
			{
				HoI4ToVic2ProvinceMap.insert(make_pair(num, V2nums));
				if (resettable)
				{
					resettableProvinces.insert(num);
				}
			}
		}
		for (auto num: V2nums)
		{
			if (num != 0)
			{
				Vic2ToHoI4ProvinceMap.insert(make_pair(num, HoI4nums));
			}
		}
	}
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