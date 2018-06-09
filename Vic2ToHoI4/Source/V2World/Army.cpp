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



#include "Army.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"



Vic2::Regiment::Regiment(std::istream& theStream)
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString nameString(theStream);
		name = Utils::convert8859_15ToUTF8(nameString.getString());
	});
	registerKeyword(std::regex("type"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString typeString(theStream);
		type = typeString.getString();
	});
	registerKeyword(std::regex("strength"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble strengthDouble(theStream);
		strength = strengthDouble.getDouble();
	});
	registerKeyword(std::regex("organisation"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble organizationDouble(theStream);
		organization = organizationDouble.getDouble();
	});
	registerKeyword(std::regex("experience"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble experienceDouble(theStream);
		experience = experienceDouble.getDouble();
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);

	if (type == "")
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
	}
}


Vic2::Army::Army(const std::string& type, std::istream& theStream):
	navy(type == "navy")
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString nameString(theStream);
		name = Utils::convert8859_15ToUTF8(nameString.getString());
	});
	registerKeyword(std::regex("location"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt locationInt(theStream);
		location = locationInt.getInt();
	});
	registerKeyword(std::regex("regiment"), [this](const std::string& unused, std::istream& theStream){
		Regiment* newRegiment = new Regiment(theStream);
		regiments.push_back(newRegiment);
	});
	registerKeyword(std::regex("ship"), [this](const std::string& unused, std::istream& theStream){
		Regiment* newShip = new Regiment(theStream);
		regiments.push_back(newShip);
	});
	registerKeyword(std::regex("supplies"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleDouble suppliesDouble(theStream);
		supplies = suppliesDouble.getDouble();
	});
	registerKeyword(std::regex("at_sea"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt locationInt(theStream);
		atSea = locationInt.getInt();
	});
	registerKeyword(std::regex("army"), [this](const std::string& type, std::istream& theStream){
		Army* transportedArmy = new Army(type, theStream);
		transportedArmies.push_back(transportedArmy);
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);

	if (location == -1)
	{
		LOG(LogLevel::Warning) << "Army or Navy " << name << " has no location";
	}
}