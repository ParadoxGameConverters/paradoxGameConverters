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


#include "V2TechSchools.h"
#include "../Log.h"
#include "../Parsers/Object.h"



vector<techSchool> initTechSchools(Object* obj, vector<string> blockedTechSchools)
{
	vector<techSchool> techSchools;

	vector<Object*> schoolObj = obj->getValue("schools");
	if (schoolObj.size() < 1)
	{
		LOG(LogLevel::Warning) << "Could not load tech schools";
	}
	
	vector<Object*> schoolsObj = schoolObj[0]->getLeaves();
	for (unsigned int i = 0; i < schoolsObj.size(); i++)
	{
		bool isBlocked = false;
		for (unsigned int j = 0; j < blockedTechSchools.size(); j++)
		{
			if(schoolsObj[i]->getKey() == blockedTechSchools[j])
			{
				isBlocked = true;
			}
		}

		if (!isBlocked)
		{
			techSchool newSchool;
			newSchool.name						= schoolsObj[i]->getKey();
			newSchool.armyInvestment		= atof( schoolsObj[i]->getValue("army_tech_research_bonus")[0]->getLeaf().c_str() );
			newSchool.commerceInvestment	= atof( schoolsObj[i]->getValue("commerce_tech_research_bonus")[0]->getLeaf().c_str() );
			newSchool.cultureInvestment	= atof( schoolsObj[i]->getValue("culture_tech_research_bonus")[0]->getLeaf().c_str() );
			newSchool.industryInvestment	= atof( schoolsObj[i]->getValue("industry_tech_research_bonus")[0]->getLeaf().c_str() );
			newSchool.navyInvestment		= atof( schoolsObj[i]->getValue("navy_tech_research_bonus")[0]->getLeaf().c_str() );
			techSchools.push_back(newSchool);
		}
	}

	return techSchools;
}


vector<string> initBlockedTechSchools(Object* obj)
{
	vector<string> blockedTechSchools;

	vector<Object*> blockedObj = obj->getLeaves();
	if (blockedObj.size() <= 0)
	{
		return blockedTechSchools;
	}

	vector<Object*> leaves = blockedObj[0]->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		blockedTechSchools.push_back(leaves[i]->getLeaf());
	}

	return blockedTechSchools;
}