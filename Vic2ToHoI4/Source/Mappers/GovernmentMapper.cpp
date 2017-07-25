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



#include "GovernmentMapper.h"
#include "../V2World/V2Country.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



governmentMapper* governmentMapper::instance = nullptr;



governmentMapper::governmentMapper()
{
	LOG(LogLevel::Info) << "Parsing governments mappings";
	auto obj = parser_UTF8::doParseFile("governmentMapping.txt");
	if (obj == nullptr)
	{
		LOG(LogLevel::Error) << "Could not parse file governmentMapping.txt";
		exit(-1);
	}

	auto governmentObjects = obj->getValue("government_mappings");
	if (governmentObjects.size() > 0)
	{
		importGovernmentMappings(governmentObjects[0]);
	}
	else
	{
		LOG(LogLevel::Error) << "governmentMapping.txt did not contain government mappings";
		exit(-1);
	}

	auto partyObjects = obj->getValue("party_mappings");
	if (partyObjects.size() > 0)
	{
		importPartyMappings(partyObjects[0]);
	}
	else
	{
		LOG(LogLevel::Error) << "governmentMapping.txt did not contain party mappings";
		exit(-1);
	}
}


void governmentMapper::importGovernmentMappings(shared_ptr<Object> obj)
{
	for (auto mapping: obj->getValue("mapping"))
	{
		governmentMapping newMapping;
		for (auto item: mapping->getLeaves())
		{
			string key = item->getKey();
			if (key == "vic")
			{
				newMapping.vic2Government = item->getLeaf();
			}
			else if (key == "hoi_gov")
			{
				newMapping.HoI4GovernmentIdeology = item->getLeaf();
			}
			else if (key == "hoi_leader")
			{
				newMapping.HoI4LeaderIdeology = item->getLeaf();
			}
			else if (key == "ruling_party")
			{
				newMapping.rulingPartyRequired = item->getLeaf();
			}
		}

		governmentMap.push_back(newMapping);
	}
}


void governmentMapper::importPartyMappings(shared_ptr<Object> obj)
{
	for (auto mapping: obj->getValue("mapping"))
	{
		partyMapping newMapping;
		for (auto item: mapping->getLeaves())
		{
			string key = item->getKey();
			if (key == "ruling_ideology")
			{
				newMapping.rulingIdeology = item->getLeaf();
			}
			else if (key == "vic2_ideology")
			{
				newMapping.vic2Ideology = item->getLeaf();
			}
			else if (key == "supported_ideology")
			{
				newMapping.supportedIdeology = item->getLeaf();
			}
		}

		partyMap.push_back(newMapping);
	}
}


string governmentMapper::GetIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology)
{
	string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology))
		{
			ideology = mapping.HoI4GovernmentIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " government " << country->getGovernment() << " to " << ideology;
	return ideology;
}


string governmentMapper::GetLeaderIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology)
{
	string ideology = "neutrality";
	for (auto mapping: governmentMap)
	{
		if (governmentMatches(mapping, country->getGovernment()) &&	rulingIdeologyMatches(mapping, Vic2RulingIdeology))
		{
			ideology = mapping.HoI4LeaderIdeology;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " leader " << country->getGovernment() << " to " << ideology;
	return ideology;
}


bool governmentMapper::governmentMatches(const governmentMapping& mapping, const string& government)
{
	return ((mapping.vic2Government == "") || (mapping.vic2Government == government));
}


bool governmentMapper::rulingIdeologyMatches(const governmentMapping& mapping, const string& rulingIdeology)
{
	return ((mapping.rulingPartyRequired == "") || (mapping.rulingPartyRequired == rulingIdeology));
}


string governmentMapper::GetSupportedIdeology(const string& rulingIdeology, const string& Vic2Ideology)
{
	string ideology = "neutrality";
	for (auto mapping: partyMap)
	{
		if ((rulingIdeology == mapping.rulingIdeology) && (Vic2Ideology == mapping.vic2Ideology))
		{
			ideology = mapping.supportedIdeology;
			break;
		}
	}

	return ideology;
}