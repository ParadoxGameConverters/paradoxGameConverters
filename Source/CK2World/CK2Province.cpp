#include "CK2Province.h"
#include "..\Parsers\Object.h"
#include "CK2Building.h"
#include "CK2Barony.h"
#include "CK2Title.h"
#include "CK2Character.h"
#include "..\Log.h"



CK2Province::CK2Province(Object* obj, map<string, CK2Title*> titles, const CK2BuildingFactory* buildingFactory, const religionGroupMapping& religionGroupMap, const cultureGroupMapping& cultureGroupMap)
{
	number = atoi( obj->getKey().c_str() );

	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if (key[0] == 'b')
		{
			CK2Barony* newBarony = new CK2Barony( leaves[i], titles[key], this, buildingFactory, religionGroupMap, cultureGroupMap);
			if (newBarony->getTitle()->getHolder()->getCapitalString() == newBarony->getTitle()->getTitleString())
			{
				newBarony->getTitle()->getHolder()->setCapital(this);
				newBarony->getTitle()->getHolder()->setPrimaryHolding(newBarony);
			}
			newBarony->getTitle()->getHolder()->addHolding(newBarony);
			baronies.push_back(newBarony);
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
		religion  = religionObj[0]->getLeaf();
	}
	else
	{
		religion = "";
	}

	techLevels.clear();
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