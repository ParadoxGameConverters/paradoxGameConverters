#include "EU3Advisor.h"
#include "EU3Province.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2Dynasty.h"
#include "..\CK2World\CK2Province.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include <fstream>



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
	int*				srcStats	= src->getStats();
	switch (jobType)
	{
		case CHANCELLOR:
			advisorType = "diplomat";
			advisorSkill	= srcStats[DIPLOMACY] / 4;
			break;
		case MARSHAL:
			advisorType = "army_reformer";
			advisorSkill	= srcStats[MARTIAL] / 4;
			break;
		case STEWARD:
			advisorType = "sheriff";
			advisorSkill	= srcStats[STEWARDSHIP] / 4;
			break;
		case SPYMASTER:
			advisorType = "spymaster";
			advisorSkill	= srcStats[INTRIGUE] / 4;
			break;
		case CHAPLAIN:
			advisorType = "theologian";
			advisorSkill	= srcStats[LEARNING] / 4;
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