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



#include "ProvinceDefinitions.h"
#include <fstream>
#include "Log.h"
#include "../Configuration.h"
using namespace std;



provinceDefinitions* provinceDefinitions::instance = nullptr;



provinceDefinitions::provinceDefinitions()
{
	ifstream definitions(Configuration::getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << Configuration::getHoI4Path() << "/map/definition.csv";
		exit(-1);
	}

	while (true)
	{
		string line;
		getline(definitions, line);
		int pos = line.find_first_of(';');
		if (pos == string::npos)
		{
			break;
		}
		int provNum = stoi(line.substr(0, pos));
		if (provNum == 0)
		{
			continue;
		}
		line = line.substr(pos + 1, line.length());

		pos = line.find_first_of(';');
		int red = stoi(line.substr(0, pos));
		line = line.substr(pos + 1, line.length());

		pos = line.find_first_of(';');
		int green = stoi(line.substr(0, pos));
		line = line.substr(pos + 1, line.length());

		pos = line.find_first_of(';');
		int blue = stoi(line.substr(0, pos));
		line = line.substr(pos + 1, line.length());

		Color provinceColor(red, green, blue);
		int colorInt = getIntFromColor(provinceColor);
		colorToProvinceMap.insert(make_pair(colorInt, provNum));

		pos = line.find_first_of(';');
		line = line.substr(0, pos);
		if (line == "land")
		{
			landProvinces.insert(provNum);
		}
	}
}


int provinceDefinitions::GetProvinceFromColor(const Color& color) const
{
	int colorInt = getIntFromColor(color);

	auto mapping = colorToProvinceMap.find(colorInt);
	if (mapping != colorToProvinceMap.end())
	{
		return mapping->second;
	}
	else
	{
		return 0;
	}
}


int provinceDefinitions::getIntFromColor(const Color& color) const
{
	int red, green, blue;
	color.GetRGB(red, green, blue);
	int colorInt = ((red & 0xFF) << 16) +
						((green & 0xFF) << 8) +
						(blue & 0xFF);

	return colorInt;
}