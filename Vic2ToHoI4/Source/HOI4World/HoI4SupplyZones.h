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



#ifndef HOI4_SUPPLY_ZONES_H
#define HOI4_SUPPLY_ZONES_H



#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;



class HoI4States;
class HoI4SupplyZone;
class Object;



class HoI4SupplyZones
{
	public:
		HoI4SupplyZones();
		void output();
		void convertSupplyZones(const HoI4States* states);

	private:
		void importStates();
		void importSupplyZone(const string& supplyZonesFile);
		void mapProvincesToSupplyZone(int ID, shared_ptr<Object> supplyAreaObj);

		map<int, vector<int>> defaultStateToProvinceMap;
		map<int, string> supplyZonesFilenames;
		map<int, HoI4SupplyZone*> supplyZones;
		map<int, int> provinceToSupplyZoneMap;
};



#endif // HOI4_SUPPLY_ZONES_H