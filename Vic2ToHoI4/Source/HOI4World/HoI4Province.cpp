/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "HoI4Province.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;



HoI4Province::HoI4Province(wstring _filename)
{
	filenames.insert(make_pair(_filename, _filename));
	coastal				= false;
	num					= 0;
	name					= L"";
	owner					= L"";
	points				= 0;
	metal					= 0.0;
	oil					= 0.0;
	rare_materials		= 0.0;
	energy				= 0.0;
	industry				= 0;
	land_fort			= 0;
	coastal_fort		= 0;
	infrastructure		= 0;
	naval_base			= 0;
	air_base				= 0;
	manpower				= 0.0;
	leadership			= 0.0;
	rawIndustry			= 0.0;
	cores.clear();

	int slash		= _filename.find_last_of(L"\\");
	int numDigits	= _filename.find_first_of(L"-") - slash - 2;
	wstring temp		= _filename.substr(slash + 1, numDigits);
	num				= _wtoi(temp.c_str());

	Object* obj;
	obj = parser_UTF8::doParseFile((wstring(L".\\blankMod\\output\\history\\provinces") + _filename).c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse .\\blankMod\\output\\history\\provinces" << _filename;
		exit(-1);
	}

	vector<Object*> leaves = obj->getLeaves();
	for (auto itr: leaves)
	{
		if (itr->getKey() == L"owner")
		{
			owner = itr->getLeaf();
			is_land = true;
		}
		else if (itr->getKey() == L"metal")
		{
			metal = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"crude_oil")
		{
			oil = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"rare_materials")
		{
			rare_materials = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"energy")
		{
			energy = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"manpower")
		{
			manpower = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"leadership")
		{
			leadership = _wtof(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"industry")
		{
			industry = _wtoi(itr->getLeaf().c_str());
		}
		else if (itr->getKey() == L"add_core")
		{
			cores.push_back(itr->getLeaf());
		}
	}
}


void HoI4Province::output() const
{
	for (auto filename: filenames)
	{
		FILE* output;
		if (_wfopen_s(&output, (L"Output\\" + Configuration::getOutputName() + L"\\history\\provinces\\" + filename.first).c_str(), L"w") != 0)
		{
			int errNum;
			_get_errno(&errNum);
			char errStr[256];
			strerror_s(errStr, sizeof(errStr), errNum);
			LOG(LogLevel::Error) << "Could not create province history file Output\\" << Configuration::getOutputName() << "\\history\\provinces\\" << filename.first << " - " << errStr;
			exit(-1);
		}
		if (owner != L"")
		{
			fwprintf_s(output, L"owner = %s\n", owner.c_str());
			fwprintf_s(output, L"controller = %s\n", owner.c_str());
		}
		for (auto core: cores)
		{
			fwprintf_s(output, L"add_core = %s\n", core.c_str());
		}
		if ((points > 0) || industry > 0)
		{
			fwprintf_s(output, L"points = %i\n", points + (industry / 2));
		}
		if (metal > 0.0)
		{
			fwprintf_s(output, L"metal = %.2f\n", metal);
		}
		if (energy > 0.0)
		{
			fwprintf_s(output, L"energy = %.2f\n", energy);
		}
		if (rare_materials > 0.0)
		{
			fwprintf_s(output, L"rare_materials = %.2f\n", rare_materials);
		}
		if (oil > 0.0)
		{
			fwprintf_s(output, L"crude_oil = %.2f\n", oil);
		}
		if (industry > 0)
		{
			fwprintf_s(output, L"industry = %i\n", industry);
		}
		if (land_fort > 0)
		{
			fwprintf_s(output, L"land_fort = %i\n", land_fort);
		}
		if (coastal_fort > 0)
		{
			fwprintf_s(output, L"coastal_fort = %i\n", coastal_fort);
		}
		if (infrastructure > 0)
		{
			fwprintf_s(output, L"infra = %i\n", infrastructure);
		}
		if (naval_base > 0)
		{
			fwprintf_s(output, L"naval_base = %i\n", naval_base);
		}
		if (air_base > 0)
		{
			fwprintf_s(output, L"air_base = %i\n", air_base);
		}
		if (manpower > 0.0)
		{
			fwprintf_s(output, L"manpower = %.2f\n", manpower);
		}
		if (leadership > 0.0)
		{
			fwprintf_s(output, L"leadership = %.2f\n", leadership);
		}
		fclose(output);
	}
}


void HoI4Province::convertFromOldProvince(const V2Province* oldProvince)
{
}


void HoI4Province::addCore(wstring newCore)
{
	// only add if unique
	if ( find(cores.begin(), cores.end(), newCore) == cores.end() )
	{
		cores.push_back(newCore);
	}
}


void HoI4Province::addFilename(wstring _filename)
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


void HoI4Province::requireNavalBase(int _min)
{
	if (coastal)
	{
		naval_base = max(naval_base, _min);
	}
}

void HoI4Province::requireAirBase(int _min)
{
	air_base = max(air_base, _min);
}

void HoI4Province::requireIndustry(int _min)
{
	industry = max(industry, _min);
}

void HoI4Province::requireCoastalFort(int _min)
{
	if (coastal)
	{
		coastal_fort = max(coastal_fort, _min);
	}
}

void HoI4Province::requireLandFort(int _min)
{
	land_fort = max(land_fort, _min);
}


void HoI4Province::requireInfrastructure(int _min)
{
	infrastructure = max(infrastructure, _min);
}
