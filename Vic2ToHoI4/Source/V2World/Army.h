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



#ifndef VIC2_ARMY_H_
#define VIC2_ARMY_H_



#include <vector>
#include <string>
#include "newParser.h"



namespace Vic2
{

class Regiment: commonItems::parser // also Ship
{
	public:
		explicit Regiment(std::istream& theStream);

		std::string getName() const { return name; }
		std::string getType() const { return type; }
		double getStrength() const { return strength; }
		double getOrganization() const { return organization; }
		double getExperience() const { return experience; }

	private:
		Regiment(const Regiment&) = delete;
		Regiment& operator=(const Regiment&) = delete;

		std::string name = "";
		std::string type = "";
		double strength = 0.0;
		double organization = 0.0;
		double experience = 0.0;
};


class Army: commonItems::parser // also Navy
{
	public:
		explicit Army(const std::string& type, std::istream& theStream);

		std::string getName() const { return name; }
		bool isNavy() const { return navy; }
		double getSupplies() const { return supplies; }
		int isAtSea() const { return atSea; }
		int getLocation() const { return location; }
		std::vector<Regiment*> getRegiments() const { return regiments; }
		std::vector<Army*> getTransportedArmies() const { return transportedArmies; }

	private:
		Army(const Army&) = delete;
		Army& operator=(const Army&) = delete;

		std::string name = "";
		int location = -1;
		std::vector<Regiment*> regiments;
		double supplies = 0.0;
		int atSea = 0;
		bool navy = false;
		std::vector<Army*> transportedArmies;
};

}



#endif // VIC2_ARMY_H_