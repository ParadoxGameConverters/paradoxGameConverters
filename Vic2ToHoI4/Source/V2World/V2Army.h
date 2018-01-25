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



#ifndef V2ARMY_H_
#define V2ARMY_H_



#include <vector>
#include <string>
#include "Object.h"
using namespace std;



class V2Regiment // also Ship
{
	public:
		explicit V2Regiment(shared_ptr<Object> obj);

		string getName() const { return name; }
		string getType() const { return type; }
		double getStrength() const { return strength; }
		double getOrganization() const { return organization; }
		double getExperience() const { return experience; }

	private:
		V2Regiment(const V2Regiment&) = delete;
		V2Regiment& operator=(const V2Regiment&) = delete;

		string name;
		string type;
		double strength;
		double organization;
		double experience;
};


class V2Army // also Navy
{
	public:
		explicit V2Army(shared_ptr<Object> obj);

		string getName() const { return name; }
		bool isNavy() const { return navy; }
		double getSupplies() const { return supplies; }
		int isAtSea() const { return at_sea; }
		int getLocation() const { return location; }
		vector<V2Regiment*> getRegiments() const { return regiments; }

	private:
		V2Army(const V2Army&) = delete;
		V2Army& operator=(const V2Army&) = delete;

		string name;
		int location;
		vector<V2Regiment*> regiments;
		double supplies;
		int at_sea;
		bool navy;
};



#endif // V2ARMY_H_