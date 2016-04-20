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



#include "GovernmentMapper.h"

#include <math.h>
#include <float.h>
#include "V2World/V2Country.h"
#include "Log.h"



governmentMapper* governmentMapper::instance = NULL;


governmentMapper::governmentMapper()
{
	reformsInitialized		= false;
	totalPoliticalReforms	= 0;
	totalSocialReforms		= 0;
}


void governmentMapper::initGovernmentMap(Object* obj)
{
	vector<Object*> links = obj->getValue("link");
	for (auto link: links)
	{
		govMapping newMapping;
		newMapping.require_political_reforms		= 0.0;
		newMapping.require_social_reforms_above	= 0.0;
		newMapping.require_social_reforms_below	= 1.0;

		vector<Object*> items = link->getLeaves();
		for (auto item : items)
		{
			string key = item->getKey();
			if (key == "vic")
			{
				newMapping.vic_gov = item->getLeaf();
			}
			else if (key == "hoi")
			{
				newMapping.hoi3_gov = item->getLeaf();
			}
			else if (key == "ruling_party")
			{
				newMapping.ruling_party_required = item->getLeaf();
			}
			else if (key == "political_reforms")
			{
				newMapping.require_political_reforms = atof(item->getLeaf().c_str());
			}
			else if (key == "social_reforms_above")
			{
				newMapping.require_social_reforms_above = atof(item->getLeaf().c_str());
			}
			else if (key == "social_reforms_below")
			{
				newMapping.require_social_reforms_below = atof(item->getLeaf().c_str());
			}
		}
		governmentMap.push_back(newMapping);
	}
}


void governmentMapper::initReforms(Object* obj)
{
	vector<Object*> topObjects = obj->getLeaves();
	for (auto topObject : topObjects)
	{
		if (topObject->getKey() == "political_reforms")
		{
			vector<Object*> reformObjs = topObject->getLeaves();
			for (auto reformObj : reformObjs)
			{
				reformTypes.insert(make_pair(reformObj->getKey(), ""));

				int reformLevelNum = 0;
				vector<Object*> reformLevelObjs = reformObj->getLeaves();
				for (auto reformLevel : reformLevelObjs)
				{
					if ((reformLevel->getKey() == "next_step_only") || (reformLevel->getKey() == "administrative"))
					{
						continue;
					}

					politicalReformScores.insert(make_pair(reformLevel->getKey(), reformLevelNum));
					reformLevelNum++;
					totalPoliticalReforms++;
				}
				totalPoliticalReforms--;
			}
		}

		if (topObject->getKey() == "social_reforms")
		{
			vector<Object*> reformObjs = topObject->getLeaves();
			for (auto reformObj : reformObjs)
			{
				reformTypes.insert(make_pair(reformObj->getKey(), ""));

				int reformLevelNum = 0;
				vector<Object*> reformLevelObjs = reformObj->getLeaves();
				for (auto reformLevel : reformLevelObjs)
				{
					if ((reformLevel->getKey() == "next_step_only") || (reformLevel->getKey() == "administrative"))
					{
						continue;
					}

					socialReformScores.insert(make_pair(reformLevel->getKey(), reformLevelNum));
					reformLevelNum++;
					totalSocialReforms++;
				}
				totalSocialReforms--;
			}
		}
	}
}


string governmentMapper::getGovernmentForCountry(const V2Country* country, const string ideology)
{
	// calculate the percent of reforms passed
	int politicalReforms	= 0;
	int socialReforms		= 0;
	auto currentReforms = country->getAllReforms();
	for (auto reform: currentReforms)
	{
		auto politicalReform = politicalReformScores.find(reform.first);
		if (politicalReform != politicalReformScores.end())
		{
			politicalReforms += politicalReform->second;
		}
		auto socialReform = socialReformScores.find(reform.first);
		if (socialReform != socialReformScores.end())
		{
			socialReforms += socialReform->second;
		}
	}
	float politicalReformsPercent = 1.0 * politicalReforms / totalPoliticalReforms;
	float socialReformsPercent		= 1.0 * socialReforms / totalSocialReforms;

	// find the goverment type
	string hoiGov;
	for (auto mapping : governmentMap)
	{
		if (
				(mapping.vic_gov == country->getGovernment()) &&
				((mapping.ruling_party_required == "") || (mapping.ruling_party_required == ideology)) &&
				(mapping.require_political_reforms <= politicalReformsPercent) &&
				(mapping.require_social_reforms_above <= socialReformsPercent) &&
				(mapping.require_social_reforms_below >= socialReformsPercent)
			)
		{
			hoiGov = mapping.hoi3_gov;
			break;
		}
	}

	LOG(LogLevel::Debug) << "Mapped " << country->getTag() << " government " << country->getGovernment() << " to " << hoiGov;

	return hoiGov;
}





