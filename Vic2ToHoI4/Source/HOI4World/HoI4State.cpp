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



HoI4State::HoI4State(int _ID, wstring _ownerTag)
{
	ID			= _ID;
	ownerTag	= _ownerTag;
}


void HoI4State::output()
{
	wstring filename(L"Output/" + Configuration::getOutputName() + L"/history/states/" + to_wstring(ID) + L"-blah.txt");
	wofstream out;
	out.open(filename);

	if (!out.is_open())
	{
		LOG(LogLevel::Error) << L"Could not open \"output/input/history/states/" + to_wstring(ID) + L"-blah.txt\"";
		exit(-1);
	}

	out << L"state={" << endl;
	out << L"\tid=" << ID << endl;
	out << L"\tname=" << "foo" << " # bar" << endl;
	out << L"\tmanpower = 1" << endl;
	out << endl;
	out << L"\tstate_category = town" << endl;
	out << L"" << endl;
	out << L"\thistory={" << endl;
	out << L"\t\towner = " << ownerTag << endl;
	//out << L"	victory_points = { 3838 1 }" << endl;
	//out << L"	buildings = {" << endl;
	//out << L"	infrastructure = 4" << endl;
	//out << L"	industrial_complex = 1" << endl;
	//out << L"	air_base = 1" << endl;
	//out << L"	3838 = {" << endl;
	//out << L"	naval_base = 3" << endl;
	//out << L"}" << endl;
	//out << L"}" << endl;
	//out << L"	add_core_of = FRA" << endl;
	out << L"\t}" << endl;
	out << endl;
	out << L"\tprovinces={" << endl;
	out << L"\t\t";
	for (auto provnum: provinces)
	{
		out << provnum << " ";
	}
	out << endl;
	out << L"\t}" << endl;
	out << L"}" << endl;

	out.close();
}