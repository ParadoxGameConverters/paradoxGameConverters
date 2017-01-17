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
#include <set>
#include <vector>
using namespace std;




class HoI4Building
{
	public:
		HoI4Building(int _stateID, double _xCoordinate, double _zCoordinate);

		friend ostream& operator << (ostream& out, HoI4Building& building);

		virtual ostream& print(ostream& out) const;

	protected:
		int stateID;
		double xCoordinate;
		double yCoordinate;
		double zCoordinate;
		double rotation;
};


class HoI4NavalBase: public HoI4Building
{
	public:
		HoI4NavalBase(int _stateID, double _xCoordinate, double _zCoordinate, int _connectingSeaProvince);

		ostream& print(ostream& out) const;

	private:
		int connectingSeaProvince;
};





class HoI4Buildings
{
	public:
		HoI4Buildings(const map<int, int>& provinceToStateIDMap);

		void output() const;

	private:
		void placeNavalBases(const map<int, int>& provinceToStateIDMap);

		multimap<int, HoI4Building*> buildings;
};




#endif // HOI4BUILDINGS_H_