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



#include "HoI4State.h"
#include <fstream>
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"



HoI4State::HoI4State(const Vic2State* _sourceState, int _ID, string _ownerTag)
{
	sourceState		= _sourceState;

	ID					= _ID;
	ownerTag			= _ownerTag;
	manpower			= 0;

	civFactories	= 0;
	milFactories	= 0;
	category			= "pastoral";
	railLevel		= 0;
	dockyards		= 0;

	navalLevel		= 0;
	navalLocation	= 0;

	airbaseLevel	= 0;
}


void HoI4State::output(string _filename)
{
	// create the file
	string filename("Output/" + Configuration::getOutputName() + "/history/states/" + _filename);
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/input/history/states/" + _filename;
		exit(-1);
	}

	// output the data
	out << "state={" << endl;
	out << "\tid=" << ID << endl;
	out << "\tname= \"STATE_" << ID << "\"" << endl;
	out << "\tmanpower = " << manpower << endl;
	out << endl;
	if (resources.size() > 0)
	{
		out << "\tresources={" << endl;
		for (auto resource: resources)
		{
			out << "\t\t" << resource.first << " = " << resource.second << endl;
		}
		out << "\t}" << endl;
	}
	out << "\tstate_category = "<< category << endl;
	out << "" << endl;
	out << "\thistory={" << endl;
	out << "\t\towner = " << ownerTag << endl;
	for (auto VP: victoryPoints)
	{
		out << "\t\tvictory_points = {" << endl;
		out << "\t\t\t" << VP.first << " " << VP.second << endl;
		out << "\t\t}" << endl;
	}
	out << "\t\tbuildings = {" << endl;
	out << "\t\t\tinfrastructure = "<< railLevel << endl;
	out << "\t\t\tindustrial_complex = " << civFactories << endl;
	out << "\t\t\tarms_factory = " << milFactories << endl;
	if (dockyards > 0)
	{
		out << "\t\t\tdockyard = " << dockyards << endl;
	}
		
	if ((navalLevel > 0) && (navalLocation > 0))
	{
		out << "\t\t\t" << navalLocation << " = {" << endl;
		out << "\t\t\t\tnaval_base = " << navalLevel << endl;
		out << "\t\t\t}" << endl;
	}
	out << "\t\t\tair_base = "<< airbaseLevel << endl;
	out << "\t\t}" << endl;
	//out << "\t}" << endl;
	for (auto core: cores)
	{
		out << "\t\tadd_core_of = " << core << endl;
	}
	out << "\t}" << endl;
	out << endl;
	out << "\tprovinces={" << endl;
	out << "\t\t";
	for (auto provnum : provinces)
	{
		out << provnum << " ";
	}
	out << endl;
	out << "\t}" << endl;
	out << "}" << endl;

	out.close();
}


void HoI4State::setNavalBase(int level, int location)
{
	if (provinces.find(location) != provinces.end())
	{
		navalLevel		= level;
		navalLocation	= location;

		dockyards		= 1;
	}
}


void HoI4State::setIndustry(int _civilianFactories, int _militaryFactories, string _category, int _railLevel)
{
	civFactories	= _civilianFactories;
	milFactories	= _militaryFactories;
	category			= _category;
	railLevel		= _railLevel;
}


void HoI4State::addCores(const vector<string>& newCores)
{
	for (auto newCore: newCores)
	{
		cores.insert(newCore);
	}
}


int HoI4State::getFirstProvinceByVic2Definition(const Vic2ToHoI4ProvinceMapping& provinceMap)
{
	auto vic2Province = sourceState->getProvinces()[0];
	auto provMapping = provinceMap.find(vic2Province);
	if (provMapping != provinceMap.end())
	{
		return provMapping->second[0];
	}
	else
	{
		return 0;
	}
}


pair<string, string> HoI4State::makeLocalisation(pair<const string, string> Vic2NameInLanguage)
{
	return make_pair(
		makeLocalisationKey(),
		makeLocalisationValue(Vic2NameInLanguage)
	);
}


string HoI4State::makeLocalisationKey()
{
	return string("STATE_") + to_string(ID);
}


string HoI4State::makeLocalisationValue(pair<const string, string> Vic2NameInLanguage)
{
	string localisedName = "";
	localisedName += Vic2NameInLanguage.second;

	return localisedName;
}


bool HoI4State::isProvinceInState(int provinceNum)
{
	return (provinces.find(provinceNum) != provinces.end());
}