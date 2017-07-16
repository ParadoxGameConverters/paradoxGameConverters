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



#include "ReformMapper.h"
#include <float.h>
#include <math.h>
#include "../V2World/V2Country.h"
#include "../Configuration.h"
#include "Log.h"
#include "Object.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParser8859_15.h"
#include "ParadoxParserUTF8.h"



reformMapper* reformMapper::instance = nullptr;



reformMapper::reformMapper()
{
	reformsInitialized		= false;
	totalPoliticalReforms	= 0;
	totalSocialReforms		= 0;

	LOG(LogLevel::Info) << "Parsing governments reforms";
	for (auto itr : Configuration::getVic2Mods())
	{
		if (Utils::DoesFileExist(Configuration::getV2Path() + "/mod/" + itr + "/common/issues.txt"))
		{
			auto obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/mod/" + itr + "/common/issues.txt"));
			if (obj != nullptr)
			{
				initReforms(obj);
				break;
			}
		}
	}
	if (!reformsInitialized)
	{
		auto obj = parser_8859_15::doParseFile((Configuration::getV2Path() + "/common/issues.txt"));
		if (obj != nullptr)
		{
			initReforms(obj);
		}
	}
}


void reformMapper::initReforms(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> topObjects = obj->getLeaves();
	for (auto topObject : topObjects)
	{
		if (topObject->getKey() == "political_reforms")
		{
			vector<shared_ptr<Object>> reformObjs = topObject->getLeaves();
			for (auto reformObj : reformObjs)
			{
				reformTypes.insert(make_pair(reformObj->getKey(), ""));

				int reformLevelNum = 0;
				vector<shared_ptr<Object>> reformLevelObjs = reformObj->getLeaves();
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
			vector<shared_ptr<Object>> reformObjs = topObject->getLeaves();
			for (auto reformObj : reformObjs)
			{
				reformTypes.insert(make_pair(reformObj->getKey(), ""));

				int reformLevelNum = 0;
				vector<shared_ptr<Object>> reformLevelObjs = reformObj->getLeaves();
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