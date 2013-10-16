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



#include "EU3Advisor.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Dynasty.h"
#include "..\CK2World\CK2Province.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include <fstream>



EU3Advisor::EU3Advisor(Object* advisorObj, const map<int, EU3Province*>& provinces)
{
	vector<Object*> nameObj = advisorObj->getValue("name");
	if (nameObj.size() > 0)
	{
		name = nameObj[0]->getLeaf();
	}

	id					= Configuration::getID();
	dynasty			= "";

	vector<Object*> typeObj = advisorObj->getValue("type");
	if (typeObj.size() > 0)
	{
		advisorType = typeObj[0]->getLeaf();
	}

	vector<Object*> skillObj = advisorObj->getValue("skill");
	if (skillObj.size() > 0)
	{
		advisorSkill = atoi( skillObj[0]->getLeaf().c_str() );
	}

	vector<Object*> locationObj = advisorObj->getValue("location");
	if (locationObj.size() > 0)
	{
		location = atoi( locationObj[0]->getLeaf().c_str() );
	}

	map<int, EU3Province*>::const_iterator homeProvince = provinces.find(location);
	if (homeProvince != provinces.end())
	{
		home = homeProvince->second->getOwner();
	}
	else
	{
		home = NULL;
		log("\tError: Trying to place %s %s in province %d, but it is not a valid province.\n", name.c_str(), dynasty.c_str(), location);
	}

	vector<Object*> dateObj = advisorObj->getValue("date");
	if (dateObj.size() > 0)
	{
		startDate = dateObj[0]->getLeaf();
	}

	vector<Object*> deathDateObj = advisorObj->getValue("death_date");
	if (deathDateObj.size() > 0)
	{
		deathDate = deathDateObj[0]->getLeaf();
	}
}


EU3Advisor::EU3Advisor(CK2Character* src, const inverseProvinceMapping& inverseProvinceMap, const map<int, EU3Province*>& provinces)
{
	name				= "";
	id					= Configuration::getID();
	dynasty			= "blank";

	name = src->getName();
	CK2Dynasty* dynPointer = src->getDynasty();
	if (dynPointer != NULL)
	{
		dynasty = dynPointer->getName();
	}
	else
	{
		log("Error: %s does not have a dynasty!\n", name.c_str());
	}

	advisorTypes	jobType	= src->getJobType();
	string			action	= src->getAction();
	int*				srcStats	= src->getStats();
	switch (jobType)
	{
		case CHANCELLOR:
			advisorSkill	= srcStats[DIPLOMACY] / 4;
			if (action == "action_improve_relations")
			{
				advisorType = "diplomat";
			}
			else if (action == "action_fabricate_claims")
			{
				advisorType = "ambassador";
			}
			else if (action == "action_sow_dissent")
			{
				advisorType = "ambassador";
			}
			else
			{
				advisorType = "philosopher";
			}
			break;
		case MARSHAL:
			advisorSkill	= srcStats[MARTIAL] / 4;
			if (action == "action_advance_mil_tech")
			{
				advisorType = "army_reformer";
			}
			else if (action == "action_train_troops")
			{
				advisorType = "recruitmaster";
			}
			else if (action == "action_assist_arrest")
			{
				advisorType = "high_judge";
			}
			else
			{
				advisorType = "grand_captain";
			}
			break;
		case STEWARD:
			advisorSkill	= srcStats[STEWARDSHIP] / 4;
			if (action == "action_squeeze_peasants")
			{
				advisorType = "sheriff";
			}
			else if (action == "action_oversee_construction")
			{
				advisorType = "alderman";
			}
			else if (action == "action_advance_eco_tech")
			{
				advisorType = "treasurer";
			}
			else
			{
				advisorType = "master_of_mint";
			}
			break;
		case SPYMASTER:
			advisorSkill	= srcStats[INTRIGUE] / 4;
			if (action == "action_uncover_plots")
			{
				advisorType = "inquisitor";
			}
			else
			{
				advisorType = "spymaster";
			}
			break;
		case CHAPLAIN:
			advisorSkill	= srcStats[LEARNING] / 4;
			if (action == "action_improve_rel_relations")
			{
				advisorType = "diplomat";
			}
			else if (action == "action_advance_cul_tech")
			{
				advisorType = "statesman";
			}
			else
			{
				advisorType = "theologian";
			}
			break;
		default:
			break;
	}

	if (advisorSkill > 6)
		advisorSkill = 6;

	if (src->getLocationNum() != -2)
	{
		inverseProvinceMapping::const_iterator mapItr = inverseProvinceMap.find(src->getLocationNum());
		if (mapItr != inverseProvinceMap.end())
		{
			vector<int> possibleLocations = mapItr->second;
			if (possibleLocations.size() > 0)
			{
				location = possibleLocations[0];
			}
			else
			{
				log("\tError: no possible EU3 locations for %s %s (currently in CK2 Province %d)\n", name.c_str(), dynasty.c_str(), src->getLocationNum());
			}
		}
		else
		{
			log("\tError: no possible EU3 locations for %s %s (currently in CK2 Province %d)\n", name.c_str(), dynasty.c_str(), src->getLocationNum());
		}


		map<int, EU3Province*>::const_iterator homeProvince = provinces.find(location);
		if (homeProvince != provinces.end())
		{
			home = homeProvince->second->getOwner();
		}
		else
		{
			home = NULL;
			log("\tError: Trying to place %s %s in province %d, but it is not a valid province.\n", name.c_str(), dynasty.c_str(), location);
		}
	}

	startDate = src->getBirthDate();
	startDate.year += 16;
}


void EU3Advisor::outputInProvince(FILE* output)
{
	fprintf(output,"\t\t\tadvisor=\n");
	fprintf(output,"\t\t\t{\n");
	if (dynasty != "")
	{
		fprintf(output,"\t\t\t\tname=\"%s %s\"\n", name.c_str(), dynasty.c_str());
	}
	else
	{
		fprintf(output,"\t\t\t\tname=\"%s\"\n", name.c_str());
	}
	fprintf(output,"\t\t\t\ttype=%s\n", advisorType.c_str());
	fprintf(output,"\t\t\t\tskill=%d\n", advisorSkill);
	fprintf(output,"\t\t\t\tlocation=%d\n", location);
	if (home != NULL)
	{
		fprintf(output,"\t\t\t\thome=\"%s\"\n", home->getTag().c_str());
	}
	fprintf(output,"\t\t\t\tdate=\"%d.%d.%d\"\n", startDate.year, startDate.month, startDate.day);
	fprintf(output,"\t\t\t\thire_date=\"1.1.1\"\n");
	fprintf(output,"\t\t\t\tmove=0\n");
	fprintf(output,"\t\t\t\tid=\n");
	fprintf(output,"\t\t\t\t{\n");
	fprintf(output,"\t\t\t\t\tid=%d\n", id);
	fprintf(output,"\t\t\t\t\ttype=39\n");
	fprintf(output,"\t\t\t\t}\n");
	fprintf(output,"\t\t\t}\n");
}


void EU3Advisor::outputInActive(FILE* output)
{
	fprintf(output, "\t\tadvisor=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=39\n");
	fprintf(output, "\t\t}\n");
}