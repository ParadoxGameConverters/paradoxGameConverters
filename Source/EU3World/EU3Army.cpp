#include "EU3Army.h"
#include "EU3Province.h"
#include "..\CK2World\CK2Army.h"
#include "..\Configuration.h"
#include "..\Log.h"
#include <fstream>



EU3Regiment::EU3Regiment(const string _type, const double _strength)
{
	id			= Configuration::getArmyID();
	name		= "";
	home		= 1;
	type		= _type;
	morale	= 0.0F;
	strength	= _strength;
}


void EU3Regiment::output(FILE* output)
{
	fprintf(output, "\t\tregiment=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=\n");
	fprintf(output, "\t\t\t{\n");
	fprintf(output, "\t\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\t\ttype=4713\n");
	fprintf(output, "\t\t\t}\n");
	fprintf(output, "\t\t\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\t\t\thome=%d\n", home);
	fprintf(output, "\t\t\ttype=\"%s\"\n", type.c_str());
	fprintf(output, "\t\t\tmorale=%f\n", morale);
	fprintf(output, "\t\t\tstrength=%f\n", strength);
	fprintf(output, "\t\t}\n");
}


EU3Army::EU3Army(const CK2Army* srcArmy, const inverseProvinceMapping inverseProvinceMap, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower)
{
	id							= Configuration::getArmyID();
	name						= srcArmy->getName();
	//previous;
	movementProgress		= srcArmy->getMovementProgress();
	vector<int> srcPath	= srcArmy->getPath();
	for(unsigned int i = 0; i < srcPath.size(); i++)
	{
		inverseProvinceMapping::const_iterator locItr = inverseProvinceMap.find(srcPath[i]);
		if ((locItr != inverseProvinceMap.end()) && (locItr->second.size() > 0))
		{
			path.push_back(locItr->second[0]);
		}
	}
	inverseProvinceMapping::const_iterator locItr = inverseProvinceMap.find(srcArmy->getLocation());
	if ((locItr != inverseProvinceMap.end()) && (locItr->second.size() > 0))
	{
		location = locItr->second[0];
	}
	else
	{
		location = 1;
	}
	map<int, EU3Province*>::iterator provItr = provinces.find(location);
	if (provItr != provinces.end())
	{
		provItr->second->setArmyHere(true);
	}

	double	currentInfantryPSE	= srcArmy->getCurrentInfantryPSE();
	double	maxInfantryPSE			= srcArmy->getMaxInfantryPSE();
	int		numInfantry				= int(maxInfantryPSE / 1000);
	double	strength;
	if (numInfantry > 0)
	{
			strength = currentInfantryPSE / (numInfantry * 1000);
	}
	else
	{
		strength = 0.0F;
	}
	if (strength > 1.0)
	{
		strength = 1.0F;
	}
	for (int i = 0; i < numInfantry; i++)
	{
		EU3Regiment* newRegiment = new EU3Regiment(infantryType, strength);
		regiments.push_back(newRegiment);
	}
	manpower += (currentInfantryPSE - (strength * numInfantry)) / 1000;

	double	currentCavalryPSE	= srcArmy->getCurrentCavalryPSE();
	double	maxCavalryPSE		= srcArmy->getMaxCavalryPSE();
	int		numCavalry			= int(maxCavalryPSE / 1000);
	if (numCavalry > 0)
	{
		strength	= currentCavalryPSE / (numCavalry * 1000);
	}
	else
	{
		strength = 0.0F;
	}
	if (strength > 1.0)
	{
		strength = 1.0F;
	}
	for (int i = 0; i < numCavalry; i++)
	{
		EU3Regiment* newRegiment = new EU3Regiment(cavalryType, strength);
		regiments.push_back(newRegiment);
	}
	manpower += (currentCavalryPSE - (strength * numCavalry)) / 1000;

	vector<int> srcHomeProvinces = srcArmy->getHomeProvinces();
	vector<int> homeProvinces;
	for (unsigned int i = 0; i < srcHomeProvinces.size(); i++)
	{
		inverseProvinceMapping::const_iterator locItr = inverseProvinceMap.find(srcHomeProvinces[i]);
		if (locItr != inverseProvinceMap.end())
		{
			for (unsigned int j = 0; j < locItr->second.size(); j++)
			{
				homeProvinces.push_back(locItr->second[j]);
			}
		}
	}
	for (unsigned int i = 0; i < regiments.size(); i++)
	{
		int homeProvince = homeProvinces[int(homeProvinces.size() * ((double)rand() / RAND_MAX))];
		regiments[i]->setHomeProvince( homeProvince );
		
		string name;
		map<int, EU3Province*>::iterator provItr = provinces.find(homeProvince);
		if (provItr != provinces.end())
		{
			int num = provItr->second->getNumRegiments();
			name	 = to_string((long long)num);

			int hundredRemainder = num % 100;
			if (hundredRemainder >= 10 && hundredRemainder <= 20)
			{
				name += "th";
			}
			else
			{
				int tenRemainder = num % 10;
				switch (tenRemainder)
				{
					case 1:
						name +=  "st";
						break;
					case 2:
						name +=  "nd";
						break;
					case 3:
						name +=  "rd";
						break;
					default:
						name +=  "th";
						break;
				}
			}

			name += " ";
			name += provItr->second->getCapital();
		}
		else
		{
			log("\t\tWarning: No home province for regiment!");
		}
		name += " Regiment";
		regiments[i]->setName(name);
	}

	//stagingProvince;
	//base;
	//at_sea;

	//leaderID;
}


void EU3Army::output(FILE* output)
{
	fprintf(output, "\tarmy=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tid=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=4713\n");
	fprintf(output, "\t\t}\n");
	fprintf(output, "\t\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\t\tmovement_progress=%f\n", movementProgress);
	fprintf(output, "\t\tpath=\n");
	fprintf(output, "\t\t{\n");
	fprintf(output, "\t\t\t");
	for (unsigned int i = 0; i < path.size(); i++)
	{
		fprintf(output, "%d\t", path[i]);
	}
	fprintf(output, "\n\t\t}\n");
	fprintf(output, "\t\tlocation=%d\n", location);
	for (unsigned int i = 0; i < regiments.size(); i++)
	{
		regiments[i]->output(output);
	}
	fprintf(output, "\t\ttarget=0\n");
	fprintf(output, "\t\tstaging_province=0\n");
	fprintf(output, "\t\tbase=1\n");
	fprintf(output, "\t}\n");
}


int EU3Army::getNumInfantry()	const
{
	int infantry = 0;
	for (unsigned int i = 0; i < regiments.size(); i++)
	{
		if (regiments[i]->getType() == "infantry")
		{
			infantry++;
		}
	}
	return infantry;
}