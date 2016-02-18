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



#include "HoI3Province.h"
#include "../Log.h"
#include "../Parsers\Object.h"
#include "../Parsers\Parser.h"
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;



HoI3Province::HoI3Province(string _filename)
{
	filenames.insert(make_pair(_filename, _filename));
	coastal				= false;
	num					= 0;
	name					= "";
	owner					= "";
	//controller			= "";
	points = 0;
	metal = 0.0;
	oil = 0.0;
	rare_materials = 0.0;
	energy = 0.0;
	industry = 0;
	land_fort = 0;
	coastal_fort = 0;
	infrastructure = 0;
	naval_base = 0;
	air_base = 0;
	manpower = 0.0;
	leadership = 0.0;
	cores.clear();

	int slash		= _filename.find_last_of("\\");
	int numDigits	= _filename.find_first_of("-") - slash - 2;
	string temp		= _filename.substr(slash + 1, numDigits);
	num				= atoi(temp.c_str());

	Object* obj;
	struct _stat st;
	obj = doParseFile((string(".\\blankMod\\output\\history\\provinces") + _filename).c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse .\\blankMod\\output\\history\\provinces" << _filename;
		exit(-1);
	}

	vector<Object*> leaves = obj->getLeaves();
	for (vector<Object*>::iterator itr = leaves.begin(); itr != leaves.end(); itr++)
	{
		if ((*itr)->getKey() == "owner")
		{
			owner = (*itr)->getLeaf();
			is_land = true;
		}
		else if ((*itr)->getKey() == "metal")
		{
			metal = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "crude_oil")
		{
			oil = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "rare_materials")
		{
			rare_materials = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "energy")
		{
			energy = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "manpower")
		{
			manpower = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "leadership")
		{
			leadership = atof((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "industry")
		{
			industry = atoi((*itr)->getLeaf().c_str());
		}
		else if ((*itr)->getKey() == "controller")
		{
			//controller = (*itr)->getLeaf().c_str();
		}
		else if ((*itr)->getKey() == "add_core")
		{
			cores.push_back((*itr)->getLeaf());
		}
		else
		{
			//log("Unknown key - %s\n", (*itr)->getKey().c_str());
		}
	}
}


void HoI3Province::output() const
{
	for (auto filename: filenames)
	{
		FILE* output;
		if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\provinces\\" + filename.first).c_str(), "w") != 0)
		{
			int errNum;
			_get_errno(&errNum);
			char errStr[256];
			strerror_s(errStr, sizeof(errStr), errNum);
			LOG(LogLevel::Error) << "Could not create province history file Output\\" << Configuration::getOutputName() << "\\history\\provinces\\" << filename.first << " - " << errStr;
			exit(-1);
		}
		if (owner != "")
		{
			fprintf_s(output, "owner = %s\n", owner.c_str());
			fprintf_s(output, "controller = %s\n", owner.c_str());
		}
		for (unsigned int i = 0; i < cores.size(); i++)
		{
			fprintf_s(output, "add_core = %s\n", cores[i].c_str());
		}
		if ((points > 0) || industry > 0)
		{
			fprintf_s(output, "points = %i\n", points + (industry / 2));
		}
		if (metal > 0.0)
		{
			fprintf_s(output, "metal = %.2f\n", metal);
		}
		if (energy > 0.0)
		{
			fprintf_s(output, "energy = %.2f\n", energy);
		}
		if (rare_materials > 0.0)
		{
			fprintf_s(output, "rare_materials = %.2f\n", rare_materials);
		}
		if (oil > 0.0)
		{
			fprintf_s(output, "crude_oil = %.2f\n", oil);
		}
		if (industry > 0)
		{
			fprintf_s(output, "industry = %i\n", industry);
		}
		if (land_fort > 0)
		{
			fprintf_s(output, "land_fort = %i\n", land_fort);
		}
		if (coastal_fort > 0)
		{
			fprintf_s(output, "coastal_fort = %i\n", coastal_fort);
		}
		if (infrastructure > 0)
		{
			fprintf_s(output, "infra = %i\n", infrastructure);
		}
		if (naval_base > 0)
		{
			fprintf_s(output, "naval_base = %i\n", naval_base);
		}
		if (air_base > 0)
		{
			fprintf_s(output, "air_base = %i\n", air_base);
		}
		if (manpower > 0.0)
		{
			fprintf_s(output, "manpower = %.2f\n", manpower);
		}
		if (leadership > 0.0)
		{
			fprintf_s(output, "leadership = %.2f\n", leadership);
		}
		fclose(output);
	}
}


void HoI3Province::convertFromOldProvince(const V2Province* oldProvince)
{
}


void HoI3Province::addCore(string newCore)
{
	// only add if unique
	if ( find(cores.begin(), cores.end(), newCore) == cores.end() )
	{
		cores.push_back(newCore);
	}
}


void HoI3Province::addFilename(string _filename)
{
	// only add if unique
	if (filenames.find(_filename) == filenames.end())
	{
		filenames.insert(make_pair(_filename, _filename));
	}
}


static string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}


void HoI3Province::requireNavalBase(int _min)
{
	if (coastal)
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
	if (coastal)
	{
		coastal_fort = max(coastal_fort, _min);
	}
}

void HoI3Province::requireLandFort(int _min)
{
	land_fort = max(land_fort, _min);
}


void HoI3Province::requireInfrastructure(int _min)
{
	infrastructure = max(infrastructure, _min);
}
