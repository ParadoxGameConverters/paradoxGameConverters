/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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



#include "CK2Province.h"
#include "..\Parsers\Object.h"
#include "CK2Building.h"
#include "CK2Barony.h"
#include "CK2Title.h"
#include "CK2Character.h"
#include "CK2Religion.h"
#include "CK2Version.h"
#include "..\Log.h"



CK2Province::CK2Province(Object* obj, map<string, CK2Title*>& titles, map<int, CK2Character*>& characters, const CK2BuildingFactory* buildingFactory, CK2Version& version)
{
	number			= atoi( obj->getKey().c_str() );
	tradePost		= false;
	tpGarrisonSize	= 0;
	tpOwner			= NULL;

	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (key[0] == 'b')
		{
			map<string, CK2Title*>::iterator title = titles.find(key);
			if (title == titles.end())
			{
				log("Error: baron-level title %s is used in your save, but does not exist in your CK2 install.\n", key.c_str());
				continue;
			}
			CK2Barony* newBarony = new CK2Barony( leaves[i], title->second, this, buildingFactory);
			if (newBarony->getTitle()->getHolder()->getCapitalString() == newBarony->getTitle()->getTitleString())
			{
				newBarony->getTitle()->getHolder()->setCapital(this);
				newBarony->getTitle()->getHolder()->setPrimaryHolding(newBarony);
			}
			newBarony->getTitle()->getHolder()->addHolding(newBarony);
			baronies.push_back(newBarony);
		}
		if (key == "tradepost")
		{
			tradePost = true;
			int ownerNum = atoi(leaves[i]->getLeaf("owner").c_str());
			map<int, CK2Character*>::iterator owner = characters.find(ownerNum);
			if (owner != characters.end())
			{
				tpOwner = owner->second->getPrimaryTitle();
			}
			vector<Object*> garrison1Obj = leaves[i]->getValue("tr_garrison_1");
			if (garrison1Obj.size() > 0)
			{
				tpGarrisonSize += 2;
			}
			vector<Object*> garrison2Obj = leaves[i]->getValue("tr_garrison_2");
			if (garrison2Obj.size() > 0)
			{
				tpGarrisonSize += 3;
			}
			vector<Object*> garrison3Obj = leaves[i]->getValue("tr_garrison_3");
			if (garrison3Obj.size() > 0)
			{
				tpGarrisonSize += 4;
			}
		}
	}

	vector<Object*> cultureObj	= obj->getValue("culture");
	if (cultureObj.size() > 0)
	{
		culture  = cultureObj[0]->getLeaf();
	}
	else
	{
		culture = "";
	}

	vector<Object*> religionObj = obj->getValue("religion");
	if (religionObj.size() > 0)
	{
		religion  = CK2Religion::getReligion(religionObj[0]->getLeaf());
	}
	else
	{
		religion = NULL;
	}

	techLevels.clear();
	if (CK2Version("1.10") > version)
	{
		vector<Object*> techObj = obj->getValue("technology");
		if (techObj.size() > 0)
		{
			vector<Object*> levelObj = techObj[0]->getValue("level");
			if (levelObj.size() > 0)
			{
				vector<string> levelStrings = levelObj[0]->getTokens();
				for (unsigned int i = 0; i < levelStrings.size(); i++)
				{
					techLevels.push_back( atoi(levelStrings[i].c_str()) );
				}
			}
			vector<Object*> progressObj = techObj[0]->getValue("progress");
			if (progressObj.size() > 0)
			{
				vector<string> progressStrings = progressObj[0]->getTokens();
				for (unsigned int i = 0; i < progressStrings.size(); i++)
				{
					techLevels[i] += 0.1 * atoi(progressStrings[i].c_str());
				}
			}
		}
	}
	else
	{
		vector<Object*> techObj = obj->getValue("technology");
		if (techObj.size() > 0)
		{
			vector<Object*> levelObj = techObj[0]->getValue("tech_levels");
			if (levelObj.size() > 0)
			{
				vector<string> levelStrings = levelObj[0]->getTokens();
				for (unsigned int i = 0; i < levelStrings.size(); i++)
				{
					techLevels.push_back( atof(levelStrings[i].c_str()) );
				}
			}
		}
	}
}