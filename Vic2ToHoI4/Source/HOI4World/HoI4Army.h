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



#ifndef HOI4_ARMY_H_
#define HOI4_ARMY_H_



#include "Division.h"
#include "DivisionTemplate.h"
#include "MilitaryMappings.h"
#include <string>
#include <map>
#include <vector>



namespace Vic2
{

class Army;

}


namespace HoI4
{

class Army
{
	public:
		Army() = default;

		void addSourceArmies(std::vector<const Vic2::Army*> _sourceArmies) { sourceArmies = _sourceArmies; }

		void convertArmies(const militaryMappings& theMilitaryMappings, int backupLocation);

		friend std::ostream& operator << (std::ostream& output, const Army& theArmy);

	private:
		void convertArmyDivisions(const militaryMappings& theMilitaryMappings, std::map<std::string, double>& BattalionsAndCompanies, int location);
		bool sufficientUnits(const std::map<std::string, double>& units, const std::map<std::string, std::string>& subs, const std::map<std::string, int>& req);

		std::vector<const Vic2::Army*> sourceArmies;
		std::vector<DivisionType> divisions;
};

std::ostream& operator << (std::ostream& output, const Army& theArmy);

}



#endif // HOI4_ARMY_H_