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



HoI4State::HoI4State(Vic2State* _sourceState, int _ID, string _ownerTag, int _manpower)
{
	sourceState		= _sourceState;

	ID					= _ID;
	ownerTag			= _ownerTag;
	manpower			= _manpower;

	civFactories	= 0;
	milFactories	= 0;
	category			= "pastoral";
	railLevel		= 0;

	navalLevel		= 0;
	navalLocation	= 0;
}


void HoI4State::output(string _filename)
{
	string filename("Output/" + Configuration::getOutputName() + "/history/states/" + _filename);
	ofstream out(filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open \"output/input/history/states/" + _filename;
		exit(-1);
	}
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
	//out << "	victory_points = { 3838 1 }" << endl;
	out << "\t\tbuildings = {" << endl;
	out << "\t\t\tinfrastructure = "<< railLevel << endl;
	out << "\t\t\tindustrial_complex = " << civFactories << endl;
	out << "\t\t\tarms_factory = " << milFactories << endl;
		
	if ((navalLevel > 0) && (navalLocation > 0))
	{
		out << "\t\t\t" << navalLocation << " = {" << endl;
		out << "\t\t\t\tnaval_base = " << navalLevel << endl;
		out << "\t\t\t}" << endl;
	}
	//out << "\t\tair_base = 1" << endl;
	out << "\t\t}" << endl;
	//out << "\t}" << endl;
	//out << "\tadd_core_of = FRA" << endl;
	out << "\t}" << endl;
	out << endl;
	out << "\tprovinces={" << endl;
	out << "\t\t";
	for (auto provnum : provinces)
	{
		out << provnum.first << " ";
	}
	out << endl;
	out << "\t}" << endl;
	out << "}" << endl;

	out.close();
}


void HoI4State::setNavalBase(int level, int location)
{
	navalLevel		= level;
	navalLocation	= location;
}


void HoI4State::setIndustry(int _civilianFactories, int _militaryFactories, string _category, int _railLevel)
{
	civFactories	= _civilianFactories;
	milFactories	= _militaryFactories;
	category			= _category;
	railLevel		= _railLevel;
}