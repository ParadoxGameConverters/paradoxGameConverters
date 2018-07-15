/*Copyright (c) 2018 The Paradox Game Converters Project

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
#include "newParser.h"
#include "ParserHelpers.h"
#include <fstream>



class region: commonItems::parser
{
	public:
		explicit region(std::istream& theStream);

		auto getID() const { return ID; }
		auto getProvinces() const { return provinces; }
		auto getWeather() const { return weather; }

	private:
		int ID = 0;
		std::vector<int> provinces;
		std::string weather;
};


region::region(std::istream& theStream)
{
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt idString(theStream);
		ID = idString.getInt();
	});
	registerKeyword(std::regex("provinces"), [this](const std::string& unused, std::istream& theStream){
		commonItems::intList provinceInts(theStream);
		provinces = provinceInts.getInts();
	});
	registerKeyword(std::regex("weather"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem weatherString(theStream);
		weather = weatherString.getString();
	});
	registerKeyword(std::regex("[a-zA-Z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


HoI4StrategicRegion::HoI4StrategicRegion(const std::string& _filename):
	filename(_filename)
{
	registerKeyword(std::regex("strategic_region"), [this](const std::string& unused, std::istream& theStream){
		region theRegion(theStream);
		ID = theRegion.getID();
		oldProvinces = theRegion.getProvinces();
		weather = theRegion.getWeather();
	});

	parseFile(theConfiguration.getHoI4Path() + "/map/strategicregions/" + _filename);
}


void HoI4StrategicRegion::output(const std::string& path) const
{
	std::ofstream out(path + filename);
	if (!out.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << path << filename;
		exit(-1);
	}

	out << "strategic_region={\n";
	out << "\tid=" << ID << "\n";
	out << "\tname=\"STRATEGICREGION_" << ID << "\"\n";
	out << "\tprovinces={\n";
	out << "\t\t";
	for (auto province: newProvinces)
	{
		out << province << " ";
	}
	out << "\n";
	out << "\t}\n";
	out << "\tweather " << weather << "\n";
	out << "}";

	out.close();
}