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



#ifndef HOI4_LEADER_H_
#define HOI4_LEADER_H_


#include <fstream>
#include <string>
#include <vector>
#include "../V2World/Leader.h"


namespace HoI4
{

class General
{
	public:
		General(const Vic2::Leader* srcLeader, const std::string& portrait);
		General(const General&) = default;

		friend std::ofstream& operator << (std::ofstream& output, const General& instance);

	private:
		General& operator=(const General&) = delete;

		std::string name;
		std::string picture;
		std::vector<std::string> traits;
		int skill = 1;
		int attackSkill = 1;
		int defenseSkill = 1;
		int planningSkill = 1;
		int logisticsSkill = 1;
};


std::ofstream& operator << (std::ofstream& output, const General& instance);


class Admiral
{
	public:
		Admiral(const Vic2::Leader* srcLeader, const std::string& portrait);
		Admiral(const Admiral&) = default;

		friend std::ofstream& operator << (std::ofstream& output, const Admiral& instance);

	private:
		Admiral& operator=(const Admiral&) = delete;

		std::string name;
		std::string picture;
		std::vector<std::string> traits;
		int skill = 1;
};


std::ofstream& operator << (std::ofstream& output, const Admiral& instance);

}



#endif	// HOI4_LEADER_H_
