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



#ifndef HOI4BUILDINGS_H_
#define HOI4BUILDINGS_H_


#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <vector>
using namespace std;



struct buildingPosition
{
	double xCoordinate;
	double yCoordinate;
	double zCoordinate;
	double rotation;
};



class HoI4Building
{
	public:
		explicit HoI4Building(int _stateID, const buildingPosition& _position);

		friend ostream& operator << (ostream& out, const HoI4Building& building);

		virtual ostream& print(ostream& out) const;

	protected:
		int stateID;
		buildingPosition position;

	private:
		HoI4Building(const HoI4Building&) = delete;
		HoI4Building& operator=(const HoI4Building&) = delete;
};


class HoI4NavalBase: public HoI4Building
{
	public:
		HoI4NavalBase(int _stateID, const buildingPosition& _position, int _connectingSeaProvince);

		ostream& print(ostream& out) const;

	private:
		HoI4NavalBase(const HoI4NavalBase&) = delete;
		HoI4NavalBase& operator=(const HoI4NavalBase&) = delete;

		int connectingSeaProvince;
};





class HoI4Buildings
{
	public:
		explicit HoI4Buildings(const map<int, int>& provinceToStateIDMap);

		void output() const;

	private:
		HoI4Buildings(const HoI4Buildings&) = delete;
		HoI4Buildings& operator=(const HoI4Buildings&) = delete;

		void importDefaultBuildings();
		void importDefaultBuilding(const string& line);
		void importDefaultNavalBase(const buildingPosition& position, const smatch& matches);

		void placeNavalBases(const map<int, int>& provinceToStateIDMap);

		multimap<int, HoI4Building*> buildings;
		map<pair<int, int>, buildingPosition> defaultNavalBases;
};




#endif // HOI4BUILDINGS_H_