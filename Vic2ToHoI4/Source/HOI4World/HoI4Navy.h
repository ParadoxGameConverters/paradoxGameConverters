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



#ifndef HOI4_NAVY_H_
#define HOI4_NAVY_H_



#include <fstream>
#include <string>
#include <vector>



namespace HoI4
{

class Ship
{
	public:
		Ship(const std::string& _name, const std::string& _type, const std::string& _equipment, const std::string& _owner);
		Ship(const Ship&) = default;

		friend std::ofstream& operator << (std::ofstream& output, const Ship& instance);

	private:
		Ship& operator=(const Ship&) = delete;

		std::string name;
		std::string type;
		std::string equipment;
		std::string owner;
};


std::ofstream& operator << (std::ofstream& output, const Ship& instance);


class Navy
{
	public:
		Navy(const std::string& _name, int _location, int _base);

		void addShip(const Ship& newShip) { ships.push_back(newShip); }

		int getNumShips() const { return ships.size(); }

		friend std::ofstream& operator << (std::ofstream& output, const Navy& instance);

	private:
		std::string name;
		int location;
		int base;
		std::vector<Ship> ships;
};


std::ofstream& operator << (std::ofstream& output, const Navy& instance);


}



#endif // HOI4_NAVY_H_
