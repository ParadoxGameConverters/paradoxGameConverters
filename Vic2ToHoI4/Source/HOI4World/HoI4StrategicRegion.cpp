/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include "../Configuration.h"
#include "HoI4StrategicRegion.h"
#include "Log.h"
#include "ParadoxParserUTF8.h"
#include <fstream>
using namespace std;



HoI4StrategicRegion::HoI4StrategicRegion(const string& _filename):
	filename(_filename),
	ID(0),
	oldProvinces(),
	newProvinces(),
	weatherObj()
{
	auto fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/map/strategicregions/" + filename);
	if (fileObj)
	{
		auto regionObj = fileObj->safeGetObject("strategic_region");
		if (regionObj != nullptr)
		{
			ID = regionObj->safeGetInt("id", ID);

			for (auto provinceString: regionObj->safeGetTokens("provinces"))
			{
				oldProvinces.push_back(stoi(provinceString));
			}

			weatherObj = regionObj->safeGetObject("weather");
		}
	}
	else
	{
		LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/map/strategicregions/" << filename;
	}
}


void HoI4StrategicRegion::output(const string& path) const
{
	ofstream out(path + filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << path << filename;
		exit(-1);
	}

	out << "strategic_region={\n";
	out << "\tid=" << ID << endl;
	out << "\tname=\"STRATEGICREGION_" << ID << "\"\n";
	out << "\tprovinces={\n";
	out << "\t\t";
	for (auto province: newProvinces)
	{
		out << province << " ";
	}
	out << endl;
	out << "\t}\n";
	out << *weatherObj;
	out << "}";

	out.close();
}