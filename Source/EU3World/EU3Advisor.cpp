#include "EU3Advisor.h"
#include "EU3Province.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Dynasty.h"
#include "..\CK2World\CK2Province.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include <fstream>



EU3Advisor::EU3Advisor()
{
	name				= "";
	id					= -1;
	dynasty			= "";
	advisorType		= "";
	advisorSkill	= -1;
	location			= -1;
	home				= "";
	startDate		= (date)"1.1.1";
}


EU3Advisor::EU3Advisor(CK2Character* src, inverseProvinceMapping inverseProvinceMap, map<int, EU3Province*> provinces, date newStartDate)
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

	if (src->getLocationNum() != -1)
	{
		vector<int> possibleLocations = inverseProvinceMap[src->getLocationNum()];
		if (possibleLocations.size() > 0)
		{
			location = possibleLocations[0];
		}
		else
		{
			log("Error: no possible EU3 locations for %s %s (currently in CK2 Province %d)\n", name.c_str(), dynasty.c_str(), src->getLocationNum());
		}

		EU3Province* homeProvince = provinces[location];
		if (homeProvince != NULL)
		{
			home = homeProvince->getOwner();
		}
		else
		{
			log("Error: Trying to place %s %s in province %d, but it is not a valid province.\n", name.c_str(), dynasty.c_str(), location);
		}
	}

	startDate = newStartDate;
}


void EU3Advisor::outputInProvince(FILE* output)
{
	fprintf(output,"			advisor=\n");
	fprintf(output,"			{\n");
	fprintf(output,"				name=\"%s %s\"\n", name.c_str(), dynasty.c_str());
	fprintf(output,"				type=%s\n", advisorType.c_str());
	fprintf(output,"				skill=%d\n", advisorSkill);
	fprintf(output,"				location=%d\n", location);
	fprintf(output,"				home=\"%s\"\n", home.c_str());
	fprintf(output,"				date=\"%d.%d.%d\"\n", startDate.year, startDate.month, startDate.day);
	fprintf(output,"				hire_date=\"1.1.1\"\n");
	fprintf(output,"				move=0\n");
	fprintf(output,"				id=\n");
	fprintf(output,"				{\n");
	fprintf(output,"					id=%d\n", id);
	fprintf(output,"					type=39\n");
	fprintf(output,"				}\n");
	fprintf(output,"			}\n");
}


void EU3Advisor::outputInActive(FILE* output)
{
	fprintf(output, "		advisor=\n");
	fprintf(output, "		{\n");
	fprintf(output, "			id=%d\n", id);
	fprintf(output, "			type=39\n");
	fprintf(output, "		}\n");
}


void EU3Advisor::setLocation(int newLocation)
{
	location = newLocation;
}


date EU3Advisor::getDate()
{
	return startDate;
}