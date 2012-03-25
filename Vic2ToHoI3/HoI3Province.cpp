#include "HoI3Province.h"
#include "Log.h"
#include <algorithm>
#include <io.h>
#include <fstream>
#include "Configuration.h"
#include "Parsers/Parser.h"

void HoI3Province::init(int newNumber)
{
	num				= newNumber;
	name			= "";

	points = 0;
	metal = oil = rare_materials = energy = manpower = leadership = 0.0;
	is_land = false;
	is_blacklisted_port = false;
	avg_mil = 0.0;

	char sNum[8];
	sprintf_s(sNum, 8, "%d", num);
	string HoI3Loc = Configuration::getHoI3Path();

	string parentDir = HoI3Loc + "\\history\\provinces\\*";
	string folder;
	struct _finddata_t	dirData;
	intptr_t			dirListing;
	if ( (dirListing = _findfirst(parentDir.c_str(), &dirData)) != -1L)
	{
		do
		{
			string filename = HoI3Loc + "\\history\\provinces\\" + dirData.name + "\\" + sNum + "*.txt";
			struct _finddata_t	fileData;
			intptr_t			fileListing;
			if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
			{
				Object* obj = doParseFile( (HoI3Loc + "\\history\\provinces\\" + dirData.name + "\\" + fileData.name).c_str() );

				vector<Object*> results = obj->getValue("owner");
				if (results.size() > 0)
				{
					is_land = true;
				}
					
				results = obj->getValue("points");
				if (results.size() > 0)
				{
					points = atoi(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("metal");
				if (results.size() > 0)
				{
					metal = atof(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("crude_oil");
				if (results.size() > 0)
				{
					oil = atof(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("rare_materials");
				if (results.size() > 0)
				{
					rare_materials = atof(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("energy");
				if (results.size() > 0)
				{
					energy = atof(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("manpower");
				if (results.size() > 0)
				{
					manpower = atof(results[0]->getLeaf().c_str());
				}

				results = obj->getValue("leadership");
				if (results.size() > 0)
				{
					leadership = atof(results[0]->getLeaf().c_str());
				}

				_findclose(fileListing);
				break;
			}
			_findclose(fileListing);
		} while (_findnext(dirListing, &dirData) == 0);
	}
	_findclose(dirListing);

	naval_base = air_base = industry = land_fort = coastal_fort = anti_air = infrastructure = 0;

	// hack for naval bases.  some coastal provinces can't have a naval base at all.
	static vector<int> port_blacklist;
	if (port_blacklist.size() == 0)
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.push_back(temp);
		}
		s.close();
	}
	for (vector<int>::iterator itr = port_blacklist.begin(); itr != port_blacklist.end(); ++itr)
	{
		if (newNumber == *itr)
		{
			is_blacklisted_port = true;
			break;
		}
	}
}

int HoI3Province::getNum()
{
	return num;
}


string HoI3Province::getName()
{
	return name;
}


void HoI3Province::setName(string newName)
{
	name = newName;
}


void HoI3Province::setOwner(string newOwner)
{
	owner = newOwner;
}


string HoI3Province::getOwner()
{
	return owner;
}


void HoI3Province::addCore(string newCore)
{
	// only add if unique
	if (std::find(cores.begin(), cores.end(), newCore) == cores.end())
		cores.push_back(newCore);
}


void HoI3Province::output(FILE * output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	// HoI doesn't store province names in the save???
	// fprintf(output, "	name=\"%s\"\n", name.c_str());
	if (owner != "")
	{
		fprintf(output, "	owner=\"%s\"\n", owner.c_str());
		fprintf(output, "	controller=\"%s\"\n", owner.c_str());
	}
	for (unsigned int i = 0; i < cores.size(); i++)
	{
		fprintf(output, "	core=\"%s\"\n", cores[i].c_str());
	}
	fprintf(output, "	manpower=%f\n", manpower);
	fprintf(output, "	leadership=%f\n", leadership);
	if (naval_base > 0)		fprintf(output, "	naval_base = { %f %f }\n",		float(naval_base), float(naval_base));
	if (air_base > 0)		fprintf(output, "	air_base = { %f %f }\n",		float(air_base), float(air_base));
	if (industry > 0)		fprintf(output, "	industry = { %f %f }\n",		float(industry), float(industry));
	if (coastal_fort > 0)	fprintf(output, "	coastal_fort = { %f %f }\n",	float(coastal_fort), float(coastal_fort));
	if (land_fort > 0)		fprintf(output, "	land_fort = { %f %f }\n",		float(land_fort), float(land_fort));
	if (anti_air > 0)		fprintf(output, "	anti_air = { %f %f }\n",		float(anti_air), float(anti_air));
	if (infrastructure > 0)	fprintf(output, "	infra = { %f %f }\n",			float(infrastructure), float(infrastructure));
	fprintf(output, "	points=%d\n", points);
	fprintf(output, "	current_producing=\n");
	fprintf(output, "	{\n");
	if (metal > 0.0)			fprintf(output, "	metal=%f\n", metal);
	if (energy > 0.0)			fprintf(output, "	energy=%f\n", energy);
	if (rare_materials > 0.0)	fprintf(output, "	rare_materials=%f\n", rare_materials);
	if (oil > 0.0)				fprintf(output, "	crude_oil=%f\n", oil);
	fprintf(output, "	}\n");
	fprintf(output, "	max_producing=\n");
	fprintf(output, "	{\n");
	if (metal > 0.0)			fprintf(output, "	metal=%f\n", metal);
	if (energy > 0.0)			fprintf(output, "	energy=%f\n", energy);
	if (rare_materials > 0.0)	fprintf(output, "	rare_materials=%f\n", rare_materials);
	if (oil > 0.0)				fprintf(output, "	crude_oil=%f\n", oil);
	fprintf(output, "	}\n");
	fprintf(output, "}\n");
}

void HoI3Province::requireNavalBase(int _min)
{
	if (is_coastal && !is_blacklisted_port)
	{
		naval_base = max(naval_base, _min);
	}
}

void HoI3Province::requireAirBase(int _min)
{
	air_base = max(air_base, _min);
}

void HoI3Province::requireIndustry(int _min)
{
	industry = max(industry, _min);
}

void HoI3Province::requireCoastalFort(int _min)
{
	if (is_coastal)
	{
		coastal_fort = max(coastal_fort, _min);
	}
}

void HoI3Province::requireLandFort(int _min)
{
	land_fort = max(land_fort, _min);
}

void HoI3Province::requireAntiAir(int _min)
{
	anti_air = max(anti_air, _min);
}

void HoI3Province::requireInfrastructure(int _min)
{
	infrastructure = max(infrastructure, _min);
}

bool HoI3Province::isLand()
{
	return is_land;
}

void HoI3Province::setCoastal(bool newVal)
{
	is_coastal = newVal;
}

bool HoI3Province::isCoastal()
{
	return is_coastal;
}

bool HoI3Province::isBlacklistedPort()
{
	return is_blacklisted_port;
}
