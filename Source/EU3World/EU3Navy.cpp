#include "EU3Navy.h"
#include "EU3Province.h"
#include "EU3Army.h"
#include "..\Configuration.h"
#include "..\CK2World\CK2Army.h"
#include "..\Log.h"


EU3Ship::EU3Ship(const string _type, const double _strength)
{
	id			= Configuration::getArmyID();
	name		= "";
	home		= 1;
	type		= _type;
	morale	= 0.0F;
	strength	= _strength;
}


void EU3Ship::output(FILE* output)
{
	fprintf(output, "\t\tship=\n");
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


EU3Navy::EU3Navy(const CK2Army* srcNavy, const inverseProvinceMapping inverseProvinceMap, const string transportType, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower)
{
	id							= Configuration::getArmyID();
	name						= srcNavy->getName();
	movementProgress		= srcNavy->getMovementProgress();
	vector<int> srcPath	= srcNavy->getPath();
	for(unsigned int i = 0; i < srcPath.size(); i++)
	{
		inverseProvinceMapping::const_iterator locItr = inverseProvinceMap.find(srcPath[i]);
		if ((locItr != inverseProvinceMap.end()) && (locItr->second.size() > 0))
		{
			path.push_back(locItr->second[0]);
		}
	}
	inverseProvinceMapping::const_iterator locItr = inverseProvinceMap.find(srcNavy->getLocation());
	if ((locItr != inverseProvinceMap.end()) && (locItr->second.size() > 0))
	{
		location = locItr->second[0];
		atSea = !provinces[location]->isLand();
	}
	else
	{
		location = 1252;
		atSea = true;
	}

	double	currentShips			= srcNavy->getCurrentShips();
	double	maxShips					= srcNavy->getMaxShips();
	int		numShips					= int(sqrt(maxShips));
	double	strength					= currentShips / (numShips * numShips);
	if (strength > 1.0)
	{
		strength = 1.0F;
	}
	for (int i = 0; i < numShips; i++)
	{
		EU3Ship* newship = new EU3Ship(transportType, strength);
		ships.push_back(newship);
	}

	vector<int> srcHomeProvinces = srcNavy->getHomeProvinces();
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

	vector<CK2Army*> srcTransported = srcNavy->getTransportedArmies();
	for (unsigned int i = 0; i < srcTransported.size(); i++)
	{
		EU3Army* newArmy = new EU3Army(srcTransported[i], inverseProvinceMap, infantryType, cavalryType, provinces, manpower);
		transportedArmies.push_back(newArmy);
	}
	int extraArmies = transportedArmies.size() - ships.size();
	if (extraArmies > 0)
	{
		for (int i = 0; i < extraArmies; i++)
		{
			EU3Ship* newship = new EU3Ship(transportType, strength);
			ships.push_back(newship);
		}
	}

	for (unsigned int i = 0; i < ships.size(); i++)
	{
		int homeProvince = homeProvinces[int(homeProvinces.size() * ((double)rand() / RAND_MAX))];
		ships[i]->setHomeProvince( homeProvince );
		
		string name;
		map<int, EU3Province*>::iterator provItr = provinces.find(homeProvince);
		if (provItr != provinces.end())
		{
			int num = provItr->second->getNumShips();
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
			log("\t\tWarning: No home province for ship!");
		}
		name += " Transport";
		ships[i]->setName(name);
	}

	//leaderID;
}


void EU3Navy::output(FILE* output)
{
	fprintf(output, "\tnavy=\n");
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
	for (unsigned int i = 0; i < ships.size(); i++)
	{
		ships[i]->output(output);
	}
	for (unsigned int i = 0; i < transportedArmies.size(); i++)
	{
		transportedArmies[i]->output(output);
	}
	if (atSea)
	{
		fprintf(output, "\t\tat_sea=1\n");
	}
	else
	{
		fprintf(output, "\t\tat_sea=0\n");
	}
	fprintf(output, "\t}\n");
}