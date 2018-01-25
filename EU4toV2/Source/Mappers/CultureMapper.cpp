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



#include "CultureMapper.h"
#include "EU4RegionMapper.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"



cultureMapper* cultureMapper::instance = nullptr;



cultureMapper::cultureMapper()
{
	LOG(LogLevel::Info) << "Parsing culture mappings";
	shared_ptr<Object> cultureObj = parser_UTF8::doParseFile("cultureMap.txt");
	if (cultureObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file cultureMap.txt";
		exit(-1);
	}
	if (cultureObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse cultureMap.txt";
		exit(-1);
	}

	shared_ptr<Object> slaveCultureObj = parser_UTF8::doParseFile("slaveCultureMap.txt");
	if (slaveCultureObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file slaveCultureMap.txt";
		exit(-1);
	}
	if (slaveCultureObj->getLeaves().size() < 1)
	{
		LOG(LogLevel::Error) << "Failed to parse slaveCultureMap.txt";
		exit(-1);
	}

	initCultureMap(cultureObj->getLeaves()[0], slaveCultureObj->getLeaves()[0]);
}


void cultureMapper::initCultureMap(shared_ptr<Object> cultureMapObj, shared_ptr<Object> slaveCultureMapObj)
{
	for (auto rule: cultureMapObj->getLeaves())
	{
		vector<cultureStruct> newRules = createNewRules(rule);
		for (auto newRule: newRules)
		{
			cultureMap.push_back(newRule);
		}
	}

	for (auto rule: slaveCultureMapObj->getLeaves())
	{
		vector<cultureStruct> newRules = createNewRules(rule);
		for (auto newRule: newRules)
		{
			slaveCultureMap.push_back(newRule);
		}
	}
}


vector<cultureStruct> cultureMapper::createNewRules(shared_ptr<Object> ruleObj)
{
	vector<cultureStruct> newRules;

	vector<string> srcCultures;
	string dstCulture;
	map<string, string> distinguishers;
	for (auto item: ruleObj->getLeaves())
	{
		if (item->getKey() == "v2")
		{
			dstCulture = item->getLeaf();
		}
		else if (item->getKey() == "eu4")
		{
			srcCultures.push_back(item->getLeaf());
		}
		else
		{
			distinguishers.insert(make_pair(item->getKey(), item->getLeaf()));
		}
	}

	for (auto srcCulture: srcCultures)
	{
		cultureStruct rule;
		rule.srcCulture = srcCulture;
		rule.dstCulture = dstCulture;
		rule.distinguishers = distinguishers;
		newRules.push_back(rule);
	}

	return newRules;
}


bool cultureMapper::CultureMatch(const string& srcCulture, string& dstCulture, const string& religion, int EU4Province, const string& ownerTag)
{
	for (auto cultureMapping: cultureMap)
	{
		if (cultureMapping.srcCulture == srcCulture)
		{
			if (distinguishersMatch(cultureMapping.distinguishers, religion, EU4Province, ownerTag))
			{
				dstCulture = cultureMapping.dstCulture;
				return true;
			}
		}
	}

	return false;
}


bool cultureMapper::SlaveCultureMatch(const string& srcCulture, string& dstCulture, const string& religion, int EU4Province, const string& ownerTag)
{
	for (auto cultureMapping: slaveCultureMap)
	{
		if (cultureMapping.srcCulture == srcCulture)
		{
			if (distinguishersMatch(cultureMapping.distinguishers, religion, EU4Province, ownerTag))
			{
				dstCulture = cultureMapping.dstCulture;
				return true;
			}
		}
	}

	return false;
}


bool cultureMapper::distinguishersMatch(const map<string, string>& distinguishers, const string& religion, int EU4Province, const string& ownerTag)
{
	for (auto currentDistinguisher: distinguishers)
	{
		if (currentDistinguisher.first == "owner")
		{
			if (ownerTag != currentDistinguisher.second)
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "religion")
		{
			if (religion != currentDistinguisher.second)
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "region")
		{
			if (!EU4RegionMapper::provinceInRegion(EU4Province, currentDistinguisher.second))
			{
				return false;
			}
		}
		else if (currentDistinguisher.first == "provinceid")
		{
			if (stoi(currentDistinguisher.second) != EU4Province)
			{
				return false;
			}
		}
		else
		{
			LOG(LogLevel::Warning) << "Unhandled distinguisher type in culture rules: " << currentDistinguisher.first;
			return false;
		}
	}

	return true;
}